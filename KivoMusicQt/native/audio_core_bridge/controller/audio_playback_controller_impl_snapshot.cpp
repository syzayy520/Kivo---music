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
// kDetailPollEvery is a class member constant (shared with openFile) — see impl.hpp.

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

    // NB: the producer pump runs on the dedicated AudioPumpWorker thread (see
    // initialize()), NOT here — this UI-thread timer only READS state for the
    // progress/format/truth UI, so UI jank can no longer starve audio.
    auto playbackResult = snapshot::PlaybackSnapshotReader::read(
        libraryGuard_->functions(),
        engineHandle_->raw(),
        timebaseSnapshotSupported_);
    if (playbackResult.isFailed()) {
        return;
    }
    lastPlaybackSnapshot_ = playbackResult.value();

    // ── 自然播放结束检测（边沿触发）──────────────────────────
    // successful_drains 是引擎生命周期累计计数，仅在自然 EOS drain 完成时 +1
    // （用户 STOP/SEEK/FLUSH 不会触发；核心 eos_drain_tests 已证实每次自然播完恰好 +1）。
    // 严格递增 == 一首自然播完 → 触发 endOfStream（onEndOfStream 负责按模式切歌）。
    // 关键顺序：先更新基线再 emit —— endOfStream 是同线程 DirectConnection，
    // onEndOfStream→openFile 会在本次轮询内同步重入；先更新基线可防止对已消费值重复触发。
    // 注：不使用 pump 的 reached_end_of_stream（那是解码器 EOF，早于尾音放完，会截断）。
    {
        const uint64_t drains = lastPlaybackSnapshot_.successfulDrains;
        if (drains > lastSuccessfulDrains_) {
            lastSuccessfulDrains_ = drains;
            emit owner_->endOfStream();
        }
    }

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

    // 详细轮询（格式/位完美），降低频率（计数器为成员，开新文件时可强制刷新）
    if (++detailPollCounter_ >= kDetailPollEvery) {
        detailPollCounter_ = 0;

        auto formatResult = snapshot::FormatSnapshotReader::read(
            libraryGuard_->functions(),
            engineHandle_->raw());
        if (formatResult.isOk()) {
            lastFormatSnapshot_ = formatResult.value();
        }

        auto truthResult = snapshot::TruthSnapshotReader::read(
            libraryGuard_->functions(),
            engineHandle_->raw(),
            truthSnapshotSupported_);
        if (truthResult.isOk()) {
            {
                std::lock_guard<std::mutex> lock(stateMutex_);
                lastTruthSnapshot_ = truthResult.value();
                bitPerfectStatus_ = lastTruthSnapshot_.lifecycleBadge();
            }
            // Emitted outside the lock (the getters re-acquire stateMutex_ on the
            // same thread). Low-frequency (detail cadence), so unconditional emit
            // is fine — truth only changes at open/stop anyway.
            emit owner_->bitPerfectStatusChanged();
            emit owner_->truthChanged();
        }
    }

    updateFromSnapshot();
}

void AudioPlaybackControllerImpl::updateFromSnapshot() {
    bool stateChanged = false;
    bool progressChanged = false;
    bool elapsedChanged = false;
    bool durationChanged = false;
    bool formatChanged = false;

    {
        std::lock_guard<std::mutex> lock(stateMutex_);

        const bool wasPlaying = playing_;
        playing_ = lastPlaybackSnapshot_.isPlaying();
        stateChanged = wasPlaying != playing_;

        // Detect each displayed value's change INDEPENDENTLY. The previous code
        // gated progress + elapsed behind a single `|Δprogress| > 0.001` flag.
        // For a 4-minute track the per-poll (50 ms) progress delta is only
        // ~0.0002, so that flag was never true → progressChanged/elapsedChanged
        // never emitted → the seek bar and time labels froze at their initial
        // values even though the underlying state advanced. Emit on ANY change.
        const double oldProgress = progress_;
        progress_ = lastPlaybackSnapshot_.progress;
        progressChanged = oldProgress != progress_;

        const QString oldElapsed = elapsedText_;
        elapsedText_ = lastPlaybackSnapshot_.positionText();
        elapsedChanged = oldElapsed != elapsedText_;

        const QString oldDuration = durationText_;
        durationText_ = lastPlaybackSnapshot_.durationText();
        durationChanged = oldDuration != durationText_;

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
    }
    if (elapsedChanged) {
        emit owner_->elapsedTextChanged();
    }
    if (durationChanged) {
        emit owner_->durationTextChanged();
    }
    if (formatChanged) {
        emit owner_->formatTextChanged();
    }
}

} // namespace kivo::qt::audio_bridge
