// =============================================================================
// Kivo Music Qt - audio_core_worker.cpp
// Thread-safe worker: all Host ABI calls execute on the worker QThread
// =============================================================================

#include "audio_core_worker.hpp"
#include "../source/file_audio_source.hpp"
#include "../state/default_open_configuration.hpp"

#include <QFileInfo>

namespace {

constexpr int kActivePollMs  = 50;
constexpr int kIdlePollMs    = 200;
constexpr int kDetailEvery   = 10;
constexpr int kMaxIdleCount  = 40;   // 40 polls * 50ms = 2s before switching to idle

// Approximate sample rate for elapsed/duration text
constexpr uint64_t kApproxSampleRate = 48000;

QString frameToTimeText(uint64_t frames, uint64_t sr = kApproxSampleRate) {
    if (sr == 0) return "--:--";
    int totalSec = static_cast<int>(frames / sr);
    return QString("%1:%2")
        .arg(totalSec / 60, 2, 10, QChar('0'))
        .arg(totalSec % 60, 2, 10, QChar('0'));
}

} // namespace

namespace kivo::qt::audio_bridge::worker {

// ── construction / destruction ────────────────────────────────────────

AudioCoreWorker::AudioCoreWorker(QObject* parent)
    : QObject(parent) {}

AudioCoreWorker::~AudioCoreWorker() {
    shutdown();
}

// ── life cycle ─────────────────────────────────────────────────────────

void AudioCoreWorker::initialize() {
    libraryGuard_ = std::make_unique<loader::AudioCoreLibraryGuard>();
    auto initResult = libraryGuard_->initialize();
    if (initResult.isFailed()) {
        ready_ = false;
        emit engineOffline(QString("DLL not found or load failed: %1")
            .arg(initResult.error().message));
        return;
    }

    auto engineResult = handle::EngineHandle::create(libraryGuard_->functions());
    if (engineResult.isFailed()) {
        ready_ = false;
        libraryGuard_.reset();
        emit engineOffline(QString("Engine creation failed: %1")
            .arg(engineResult.error().message));
        return;
    }

    engineHandle_ = std::move(engineResult.value());
    pollTimer_ = new QTimer(this);
    pollTimer_->setInterval(kActivePollMs);
    connect(pollTimer_, &QTimer::timeout, this, &AudioCoreWorker::pollStep);

    ready_ = true;
    emit engineReady();
}

void AudioCoreWorker::shutdown() {
    if (pollTimer_) {
        pollTimer_->stop();
        delete pollTimer_;
        pollTimer_ = nullptr;
    }
    engineHandle_.reset();
    libraryGuard_.reset();
    ready_ = false;
}

// ── command slots (called from controller, queued) ─────────────────────

void AudioCoreWorker::play() {
    enqueueCommand({WorkerCommandKind::Play});
}

void AudioCoreWorker::pause() {
    enqueueCommand({WorkerCommandKind::Pause});
}

void AudioCoreWorker::stop() {
    enqueueCommand({WorkerCommandKind::Stop});
}

void AudioCoreWorker::seekTo(double progress) {
    WorkerCommand cmd{WorkerCommandKind::Seek};
    cmd.seekProgress = progress;
    enqueueCommand(cmd);
}

void AudioCoreWorker::skipBy(int seconds) {
    WorkerCommand cmd{WorkerCommandKind::Skip};
    cmd.skipSeconds = seconds;
    enqueueCommand(cmd);
}

void AudioCoreWorker::setVolume(double volume) {
    WorkerCommand cmd{WorkerCommandKind::SetVolume};
    cmd.volume = volume;
    enqueueCommand(cmd);
}

void AudioCoreWorker::openFile(const QString& filePath) {
    if (!ready_ || !engineHandle_ || !libraryGuard_) {
        emit workerError("Audio engine not initialized");
        return;
    }

    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile() || !fileInfo.isReadable()) {
        emit workerError(QString("Cannot open file: %1").arg(filePath));
        return;
    }

    emit loadingChanged(true);

    ++sessionGen_;
    ++sourceGen_;
    ++nextCommandId_;

    auto sourceLease = source::FileAudioSourceLease::open(
        filePath, sourceGen_, sourceGen_);
    if (sourceLease.isFailed()) {
        emit loadingChanged(false);
        emit workerError(QString("Source open failed: %1")
            .arg(sourceLease.error().message));
        return;
    }

    auto config = state::makeDefaultOpenConfiguration();
    auto result = libraryGuard_->functions().openSource(
        engineHandle_->raw(), nextCommandId_, sessionGen_,
        &sourceLease.value().abiSource(), &config);

    if (result != KIVO_AUDIO_RESULT_OK) {
        emit loadingChanged(false);
        notifyFailure("openSource", result, filePath);
        return;
    }

    sourceLease.value().releaseToHost();

    if (pollTimer_) {
        pollTimer_->setInterval(kActivePollMs);
        pollTimer_->start();
    }

    // Auto-play after open
    ++nextCommandId_;
    submitToEngine(KIVO_AUDIO_COMMAND_RESUME, 0, "auto-play");

    emit fileOpened();
    emit loadingChanged(false);
}

// ── polling (runs on worker QThread via QTimer) ────────────────────────

void AudioCoreWorker::pollStep() {
    // Process any pending command
    {
        std::lock_guard<std::mutex> lock(cmdMutex_);
        if (hasPending_) {
            hasPending_ = false;
            WorkerCommand cmd = pending_;

            switch (cmd.kind) {
            case WorkerCommandKind::Play:
                ++nextCommandId_;
                submitToEngine(KIVO_AUDIO_COMMAND_RESUME, 0, "play");
                if (pollTimer_) {
                    pollTimer_->setInterval(kActivePollMs);
                    pollTimer_->start();
                }
                break;
            case WorkerCommandKind::Pause:
                ++nextCommandId_;
                submitToEngine(KIVO_AUDIO_COMMAND_PAUSE, 0, "pause");
                break;
            case WorkerCommandKind::Stop:
                ++nextCommandId_;
                submitToEngine(KIVO_AUDIO_COMMAND_STOP, 0, "stop");
                if (pollTimer_) pollTimer_->stop();
                lastPlayback_ = {};
                wasPlaying_ = false;
                emit playbackSnapshot({true, false, 0.0, "0:00", "--:--"});
                break;
            case WorkerCommandKind::Seek: {
                ++nextCommandId_;
                auto frameOffset = static_cast<uint64_t>(cmd.seekProgress * kApproxSampleRate * 300.0);
                submitToEngine(KIVO_AUDIO_COMMAND_SEEK, frameOffset, "seek");
                if (pollTimer_) {
                    pollTimer_->setInterval(kActivePollMs);
                    pollTimer_->start();
                }
                break;
            }
            case WorkerCommandKind::Skip: {
                ++nextCommandId_;
                auto delta = static_cast<int64_t>(cmd.skipSeconds) * static_cast<int64_t>(kApproxSampleRate);
                submitToEngine(KIVO_AUDIO_COMMAND_SEEK,
                    static_cast<uint64_t>(delta < 0 ? 0 : delta), "skip");
                if (pollTimer_) {
                    pollTimer_->setInterval(kActivePollMs);
                    pollTimer_->start();
                }
                break;
            }
            case WorkerCommandKind::SetVolume:
                volume_ = cmd.volume;
                break;
            default:
                break;
            }
        }
    }

    // Pump and read snapshots
    pumpOnce();
    readAndEmitSnapshots();
}

// ── Host ABI helpers ───────────────────────────────────────────────────

bool AudioCoreWorker::submitToEngine(uint32_t kind, uint64_t seekFrame,
                                     const char* label) {
    if (!ready_ || !engineHandle_ || !libraryGuard_) return false;

    kivo_audio_command_v1 cmd{};
    cmd.struct_size   = sizeof(cmd);
    cmd.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    cmd.command_id    = nextCommandId_;
    cmd.session_generation = sessionGen_;
    cmd.kind          = static_cast<kivo_audio_command_kind>(kind);
    cmd.requested_frame = seekFrame;

    auto result = libraryGuard_->functions().submitCommand(
        engineHandle_->raw(), &cmd);
    if (result != KIVO_AUDIO_RESULT_OK) {
        emit workerError(
            QString("%1 failed: result=%2").arg(label).arg(static_cast<int>(result)));
        return false;
    }
    return true;
}

void AudioCoreWorker::pumpOnce() {
    if (!ready_ || !engineHandle_ || !libraryGuard_) return;

    const auto& fn = libraryGuard_->functions();
    kivo_audio_pump_request_v1 req{};
    req.struct_size = sizeof(req);
    req.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    req.maximum_steps = 4;

    kivo_audio_pump_result_v1 res{};
    res.struct_size = sizeof(res);
    res.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;

    fn.pump(engineHandle_->raw(), &req, &res);
}

void AudioCoreWorker::readAndEmitSnapshots() {
    if (!ready_ || !engineHandle_ || !libraryGuard_) return;

    const auto& fn = libraryGuard_->functions();

    // Playback snapshot (every poll)
    {
        kivo_audio_snapshot_v1 snap{};
        snap.struct_size = sizeof(snap);
        snap.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
        if (fn.getSnapshot(engineHandle_->raw(), &snap) == KIVO_AUDIO_RESULT_OK) {

            WorkerPlaybackSnapshot wsnap;
            wsnap.valid = true;
            wsnap.isPlaying = (snap.state & 1u) != 0u;

            double progress = 0.0;
            if (snap.decoded_frames > 0) {
                progress = static_cast<double>(snap.rendered_position_frames)
                         / static_cast<double>(snap.decoded_frames);
                progress = std::clamp(progress, 0.0, 1.0);
            }
            wsnap.progress = progress;
            wsnap.elapsedText = frameToTimeText(snap.rendered_position_frames);

            emit playbackSnapshot(wsnap);

            // Dynamic polling interval
            bool nowPlaying = wsnap.isPlaying;
            if (nowPlaying != wasPlaying_) {
                if (nowPlaying) {
                    idleCounter_ = 0;
                    if (pollTimer_) pollTimer_->setInterval(kActivePollMs);
                } else {
                    if (pollTimer_) pollTimer_->setInterval(kIdlePollMs);
                }
                wasPlaying_ = nowPlaying;
            } else if (!nowPlaying) {
                ++idleCounter_;
                if (idleCounter_ > kMaxIdleCount && pollTimer_
                    && pollTimer_->interval() != kIdlePollMs) {
                    pollTimer_->setInterval(kIdlePollMs);
                }
            }
        }
    }

    // Detail snapshots (every kDetailEvery polls)
    ++pollCounter_;
    if (pollCounter_ % kDetailEvery == 0) {
        kivo_audio_snapshot_v1 snap{};
        snap.struct_size = sizeof(snap);
        snap.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
        if (fn.getSnapshot(engineHandle_->raw(), &snap) == KIVO_AUDIO_RESULT_OK) {
            // Format
            {
                WorkerFormatSnapshot fmt;
                fmt.valid = true;
                if (snap.active != 0u) {
                    fmt.formatText = "Playing";
                } else if (snap.successful_opens > 0u) {
                    fmt.formatText = "Ready";
                } else {
                    fmt.formatText = "Choose music";
                }
                emit formatSnapshot(fmt);
            }
            // Truth (bit-perfect)
            {
                WorkerTruthSnapshot truth;
                truth.valid = true;
                truth.bitPerfectStatus = (snap.render_underrun_events == 0)
                    ? "Bit-Perfect" : "Processed";
                emit truthSnapshot(truth);
            }
        }
    }
}

// ── helpers ────────────────────────────────────────────────────────────

void AudioCoreWorker::notifyFailure(const QString& label, int code,
                                     const QString& detail) {
    emit workerResult(WorkerResult::failure(label, code,
        QString("%1 failed (code %2): %3").arg(label).arg(code).arg(detail)));
}

void AudioCoreWorker::enqueueCommand(WorkerCommand cmd) {
    std::lock_guard<std::mutex> lock(cmdMutex_);
    pending_ = cmd;
    hasPending_ = true;
}

} // namespace kivo::qt::audio_bridge::worker
