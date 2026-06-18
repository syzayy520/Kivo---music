// =============================================================================
// Kivo Music Qt - truth_snapshot.cpp
// AudioCoreBridge: evidence-gated audio-truth projection
// =============================================================================

#include "truth_snapshot.hpp"

namespace kivo::qt::audio_bridge::snapshot {

namespace {

QString unknown() { return QStringLiteral("Unknown"); }

// Aggregate of the four "conversion" evidence bits and the four "processing"
// bits, as bitmasks over the pinned bool-bit indices.
constexpr uint64_t bit(int index) {
    return static_cast<uint64_t>(1) << index;
}
constexpr uint64_t kConversionBits =
    bit(KIVO_AUDIO_TRUTH_BOOL_RESAMPLER_ACTIVE)
    | bit(KIVO_AUDIO_TRUTH_BOOL_SAMPLE_FORMAT_CONVERSION)
    | bit(KIVO_AUDIO_TRUTH_BOOL_BIT_DEPTH_CONVERSION)
    | bit(KIVO_AUDIO_TRUTH_BOOL_CHANNEL_REMIX);
constexpr uint64_t kProcessingBits =
    bit(KIVO_AUDIO_TRUTH_BOOL_REPLAY_GAIN_ACTIVE)
    | bit(KIVO_AUDIO_TRUTH_BOOL_VOLUME_ACTIVE)
    | bit(KIVO_AUDIO_TRUTH_BOOL_DSP_ACTIVE)
    | bit(KIVO_AUDIO_TRUTH_BOOL_DITHER_ACTIVE);

} // namespace

QString TruthSnapshot::lifecycleBadge() const {
    switch (bitPerfectStatus) {
    case BitPerfectStatus::CoreReady: return QStringLiteral("Core Ready");
    case BitPerfectStatus::Active:    return QStringLiteral("Signal Active");
    case BitPerfectStatus::Failed:    return QStringLiteral("Audio Error");
    case BitPerfectStatus::Unknown:
    default:                          return QStringLiteral("--");
    }
}

bool TruthSnapshot::isBitPerfectConfirmed() const {
    // Defensive AND of every condition, not just the verdict byte — an
    // incoherent flat tail can never be read as bit-perfect.
    return truthAvailable
        && evidenceComplete
        && verdict == Verdict::BitPerfect
        && actualMode == KIVO_AUDIO_TRUTH_MODE_EXCLUSIVE
        && engineKnown && !engineInPath
        && conversionKnown && !conversionActive
        && processingKnown && !processingActive;
}

QString TruthSnapshot::verdictText() const {
    if (isBitPerfectConfirmed()) {
        return QStringLiteral("Bit-Perfect");
    }
    if (truthAvailable && evidenceComplete && verdict == Verdict::NotBitPerfect) {
        return QStringLiteral("Not Bit-Perfect");
    }
    return unknown();
}

QString TruthSnapshot::requestedModeText() const {
    if (!truthAvailable || !evidenceComplete) return unknown();
    if (requestedMode == KIVO_AUDIO_TRUTH_MODE_EXCLUSIVE)
        return QStringLiteral("Exclusive");
    if (requestedMode == KIVO_AUDIO_TRUTH_MODE_SHARED)
        return QStringLiteral("Shared");
    return unknown();
}

QString TruthSnapshot::actualModeText() const {
    if (!truthAvailable || !evidenceComplete) return unknown();
    if (actualMode == KIVO_AUDIO_TRUTH_MODE_EXCLUSIVE)
        return QStringLiteral("Exclusive");
    if (actualMode == KIVO_AUDIO_TRUTH_MODE_SHARED)
        return QStringLiteral("Shared");
    return unknown();
}

QString TruthSnapshot::sampleRateText() const {
    if (!truthAvailable || !evidenceComplete || sampleRate == 0) return unknown();
    if (sampleRate % 1000u == 0u) {
        return QStringLiteral("%1 kHz").arg(sampleRate / 1000u);
    }
    return QStringLiteral("%1 kHz")
        .arg(static_cast<double>(sampleRate) / 1000.0, 0, 'f', 1);
}

QString TruthSnapshot::bitDepthText() const {
    if (!truthAvailable || !evidenceComplete || bitsPerSample == 0) return unknown();
    if (containerBits != 0 && containerBits != bitsPerSample) {
        return QStringLiteral("%1-bit (%2-bit container)")
            .arg(bitsPerSample).arg(containerBits);
    }
    return QStringLiteral("%1-bit").arg(bitsPerSample);
}

namespace {
QString triState(bool available, bool complete, bool known, bool value) {
    if (!available || !complete || !known) return QStringLiteral("Unknown");
    return value ? QStringLiteral("Yes") : QStringLiteral("No");
}
} // namespace

QString TruthSnapshot::engineInPathText() const {
    return triState(truthAvailable, evidenceComplete, engineKnown, engineInPath);
}
QString TruthSnapshot::conversionActiveText() const {
    return triState(
        truthAvailable, evidenceComplete, conversionKnown, conversionActive);
}
QString TruthSnapshot::processingActiveText() const {
    return triState(
        truthAvailable, evidenceComplete, processingKnown, processingActive);
}

QString TruthSnapshot::rejectionReasonText() const {
    // Explain ONLY a complete, rejected verdict (Iron Rule 20: explainable).
    if (!truthAvailable || !evidenceComplete || verdict != Verdict::NotBitPerfect) {
        return QString();
    }
    switch (rejectionCategory) {
    case KIVO_AUDIO_BP_REJECT_FORMAT_MISMATCH:
        return QStringLiteral("Output format differs from the source");
    case KIVO_AUDIO_BP_REJECT_SAMPLE_RATE_MISMATCH:
        return QStringLiteral("Sample rate is being converted");
    case KIVO_AUDIO_BP_REJECT_CHANNEL_LAYOUT_MISMATCH:
        return QStringLiteral("Channel layout is being remapped");
    case KIVO_AUDIO_BP_REJECT_PROCESSING_ACTIVE:
        return QStringLiteral("Audio processing is active");
    case KIVO_AUDIO_BP_REJECT_ENGINE_PARTICIPATION:
        return QStringLiteral("The system mixer is in the path");
    case KIVO_AUDIO_BP_REJECT_POLICY_FORBIDDEN:
        return QStringLiteral("Bit-perfect is not enabled");
    case KIVO_AUDIO_BP_REJECT_SHARED_MODE:
        return QStringLiteral("Output is in shared mode");
    case KIVO_AUDIO_BP_REJECT_MODE_MISMATCH:
        return QStringLiteral("Requested output mode was not granted");
    case KIVO_AUDIO_BP_REJECT_RESAMPLING_ACTIVE:
        return QStringLiteral("Resampling is active");
    case KIVO_AUDIO_BP_REJECT_CONVERSION_ACTIVE:
        return QStringLiteral("Sample-format conversion is active");
    case KIVO_AUDIO_BP_REJECT_REPLAY_GAIN_ACTIVE:
        return QStringLiteral("ReplayGain is active");
    case KIVO_AUDIO_BP_REJECT_VOLUME_ACTIVE:
        return QStringLiteral("Software volume is active");
    case KIVO_AUDIO_BP_REJECT_DSP_ACTIVE:
        return QStringLiteral("A DSP effect is active");
    case KIVO_AUDIO_BP_REJECT_DITHER_ACTIVE:
        return QStringLiteral("Dither is active");
    default:
        return QStringLiteral("Not bit-perfect");
    }
}

error::BridgeResult<TruthSnapshot> TruthSnapshotReader::read(
    const loader::AudioCoreFunctions& functions,
    kivo_audio_handle engine,
    bool truthCapabilitySupported) {
    if (!functions.getSnapshot) {
        return error::BridgeResult<TruthSnapshot>::failed(
            error::BridgeError{error::BridgeErrorCategory::Snapshot,
                "getSnapshot function not available", 0});
    }
    if (engine == KIVO_AUDIO_INVALID_HANDLE) {
        return error::BridgeResult<TruthSnapshot>::failed(
            error::BridgeError::invalidState("Engine handle is invalid"));
    }

    TruthSnapshot truth{};

    // Coarse lifecycle badge (unchanged heuristic; never claims bit-perfect).
    kivo_audio_snapshot_v1 snapshot{};
    snapshot.struct_size = sizeof(kivo_audio_snapshot_v1);
    snapshot.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    const auto result = functions.getSnapshot(engine, &snapshot);
    if (result != KIVO_AUDIO_RESULT_OK) {
        return error::BridgeResult<TruthSnapshot>::failed(
            error::BridgeError{error::BridgeErrorCategory::Snapshot,
                "Failed to read snapshot", static_cast<int>(result)});
    }
    if (snapshot.state == KIVO_AUDIO_STATE_FAILED) {
        truth.bitPerfectStatus = BitPerfectStatus::Failed;
    } else if (snapshot.active != 0u) {
        truth.bitPerfectStatus = BitPerfectStatus::Active;
    } else {
        truth.bitPerfectStatus = BitPerfectStatus::CoreReady;
    }

    // Structured evidence-gated truth (ABI 1.2.0). Absent capability/getter =>
    // everything stays Unknown (no overclaim possible).
    if (truthCapabilitySupported && functions.getTruth != nullptr) {
        kivo_audio_truth_v1 t{};
        t.struct_size = sizeof(kivo_audio_truth_v1);
        t.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
        if (functions.getTruth(engine, &t) == KIVO_AUDIO_RESULT_OK) {
            truth.truthAvailable = true;
            truth.evidenceComplete = t.evidence_complete != 0u;
            switch (t.verdict) {
            case KIVO_AUDIO_BIT_PERFECT_VERDICT_BIT_PERFECT:
                truth.verdict = TruthSnapshot::Verdict::BitPerfect; break;
            case KIVO_AUDIO_BIT_PERFECT_VERDICT_REJECTED:
                truth.verdict = TruthSnapshot::Verdict::NotBitPerfect; break;
            default:
                truth.verdict = TruthSnapshot::Verdict::Unknown; break;
            }
            truth.rejectionCategory = t.rejection_category;
            truth.requestedMode = t.requested_mode;
            truth.actualMode = t.actual_mode;
            truth.sampleRate = t.render_format.sample_rate;
            truth.bitsPerSample = t.render_format.bits_per_sample;
            truth.containerBits = t.render_format.container_bits;

            const uint64_t known = t.known_mask;
            const uint64_t vals = t.bool_mask;
            const uint64_t engineBit =
                bit(KIVO_AUDIO_TRUTH_BOOL_HOST_ENGINE_IN_PATH);
            truth.engineKnown = (known & engineBit) != 0u;
            truth.engineInPath = (vals & engineBit) != 0u;
            truth.conversionKnown = (known & kConversionBits) == kConversionBits;
            truth.conversionActive = (vals & kConversionBits) != 0u;
            truth.processingKnown = (known & kProcessingBits) == kProcessingBits;
            truth.processingActive = (vals & kProcessingBits) != 0u;
        }
    }

    return error::BridgeResult<TruthSnapshot>::ok(truth);
}

} // namespace kivo::qt::audio_bridge::snapshot
