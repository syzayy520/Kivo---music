// =============================================================================
// Kivo Music Qt - audio_playback_controller_impl_snapshot.cpp
// 职责: 快照轮询和 UI 状态投影
// 性能: 播放中 50ms → 空闲时 200ms，减少 75% CPU 占用
// =============================================================================

#include "audio_playback_controller_impl.hpp"
#include "../snapshot/truth_snapshot.hpp"

#include <cmath>

namespace kivo::qt::audio_bridge {

// 轮询间隔常量（ms）
static constexpr int kActivePollIntervalMs = 50;    // 播放中：快速更新进度
static constexpr int kIdlePollIntervalMs = 200;     // 暂停/停止：节能
static constexpr int kIdleTransitionDelayMs = 2000;  // 空闲 2 秒后切换为节能间隔
static constexpr int kDetailPollEvery = 10;          // 每 10 次轮询更新格式/位完美

void AudioPlaybackControllerImpl::startSnapshotPolling() {
    if (snapshotTimer_ != nullptr && !snapshotTimer_->isActive()) {
        snapshotTimer_->start(kActivePollIntervalMs);
        idleCounter_ = 0;
    }
}

void AudioPlaybackControllerImpl::stopSnapshotPolling() {
    if (snapshotTimer_ != nullptr && snapshotTimer_->isActive()) {
        snapshotTimer_->stop();
    }
}

void AudioPlaybackControllerImpl::pollSnapshot() {
    if (!engineHandle_ || !libraryGuard_) {
        return;
    }

    (void)pumpOnce();

    auto playbackResult = snapshot::PlaybackSnapshotReader::read(
        libraryGuard_->functions(),
        engineHandle_->raw());
    if (playbackResult.isFailed()) {
        return;
    }
    lastPlaybackSnapshot_ = playbackResult.value();

    // ── 动态轮询间隔 ──────────────────────────────────────
    const bool nowPlaying = lastPlaybackSnapshot_.isPlaying();
    {
        std::lock_guard<std::mutex> lock(stateMutex_);
        if (nowPlaying) {
            idleCounter_ = 0;
            if (snapshotTimer_->interval() != kActivePollIntervalMs) {
                snapshotTimer_->setInterval(kActivePollIntervalMs);
            }
        } else {
            idleCounter_ += snapshotTimer_->interval();
            if (idleCounter_ >= kIdleTransitionDelayMs &&
                snapshotTimer_->interval() != kIdlePollIntervalMs) {
                snapshotTimer_->setInterval(kIdlePollIntervalMs);
            }
        }
    }

    // 详细轮询（格式/位完美），降低频率
    static int detailPollCounter = 0;
    if (++detailPollCounter >= kDetailPollEvery) {
        detailPollCounter = 0;

        auto formatResult = snapshot::FormatSnapshotReader::read(
            libraryGuard_->functions(),
            engineHandle_->raw());
        if (formatResult.isOk()) {
            lastFormatSnapshot_ = formatResult.value();
        }

        auto truthResult = snapshot::TruthSnapshotReader::read(
            libraryGuard_->functions(),
            engineHandle_->raw());
        if (truthResult.isOk()) {
            std::lock_guard<std::mutex> lock(stateMutex_);
            bitPerfectStatus_ = truthResult.value().statusBadge();
        }
    }

    updateFromSnapshot();
}

void AudioPlaybackControllerImpl::updateFromSnapshot() {
    bool stateChanged = false;
    bool progressChanged = false;
    bool formatChanged = false;

    {
        std::lock_guard<std::mutex> lock(stateMutex_);

        const bool wasPlaying = playing_;
        playing_ = lastPlaybackSnapshot_.isPlaying();
        stateChanged = wasPlaying != playing_;

        const double oldProgress = progress_;
        progress_ = lastPlaybackSnapshot_.progress;
        progressChanged = std::abs(oldProgress - progress_) > 0.001;

        elapsedText_ = lastPlaybackSnapshot_.positionText();
        durationText_ = lastPlaybackSnapshot_.durationText();
        duration_ = lastPlaybackSnapshot_.duration;

        const QString oldFormat = formatText_;
        formatText_ = lastFormatSnapshot_.formatText();
        formatChanged = oldFormat != formatText_;
    }

    if (stateChanged) {
        emit owner_->playingChanged();
    }
    if (progressChanged) {
        emit owner_->progressChanged();
        emit owner_->elapsedTextChanged();
    }
    if (formatChanged) {
        emit owner_->formatTextChanged();
    }
}

} // namespace kivo::qt::audio_bridge
