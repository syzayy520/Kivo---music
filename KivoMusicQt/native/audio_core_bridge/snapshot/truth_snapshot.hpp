// =============================================================================
// Kivo Music Qt - truth_snapshot.hpp
// AudioCoreBridge: evidence-gated audio-truth projection (ABI 1.2.0)
// =============================================================================

#pragma once

#include "../error/bridge_error.hpp"
#include "../loader/audio_core_function_resolver.hpp"
#include <QString>
#include <cstdint>

namespace kivo::qt::audio_bridge::snapshot {

// Coarse lifecycle badge derived from state/active. Does NOT claim bit-perfect.
enum class BitPerfectStatus {
    Unknown,
    CoreReady,
    Active,
    Failed
};

// Structured, evidence-gated truth. Every display helper returns "Unknown"
// unless the evidence proves the value — the single overclaim chokepoint. The
// product NEVER shows a sample rate / bit depth / "Bit-Perfect" it cannot back.
struct TruthSnapshot {
    // --- Coarse lifecycle (backward compatible) ---
    BitPerfectStatus bitPerfectStatus = BitPerfectStatus::Unknown;
    [[nodiscard]] QString lifecycleBadge() const;

    // --- Structured truth (from kivo_audio_get_truth) ---
    enum class Verdict { Unknown, BitPerfect, NotBitPerfect };

    bool truthAvailable = false;   // getter present + capability + read OK
    bool evidenceComplete = false;
    Verdict verdict = Verdict::Unknown;
    uint32_t rejectionCategory = 0;
    uint32_t requestedMode = 0xFFFFFFFFu;  // KIVO_AUDIO_TRUTH_MODE_*
    uint32_t actualMode = 0xFFFFFFFFu;
    uint32_t sampleRate = 0;               // render (actual output) rate, Hz
    uint8_t bitsPerSample = 0;
    uint8_t containerBits = 0;
    bool engineKnown = false, engineInPath = false;
    bool conversionKnown = false, conversionActive = false;
    bool processingKnown = false, processingActive = false;

    // --- Honesty helpers ---
    // The ONLY place that may say "Bit-Perfect": requires complete evidence,
    // a positive verdict, exclusive output, and no engine/conversion/processing.
    [[nodiscard]] bool isBitPerfectConfirmed() const;
    [[nodiscard]] QString verdictText() const;
    [[nodiscard]] QString requestedModeText() const;
    [[nodiscard]] QString actualModeText() const;
    [[nodiscard]] QString sampleRateText() const;
    [[nodiscard]] QString bitDepthText() const;
    [[nodiscard]] QString engineInPathText() const;
    [[nodiscard]] QString conversionActiveText() const;
    [[nodiscard]] QString processingActiveText() const;
    [[nodiscard]] QString rejectionReasonText() const;
};

class TruthSnapshotReader final {
public:
    // truthCapabilitySupported gates the ABI 1.2.0 getter; when false (older
    // core or capability absent) the structured fields stay Unknown.
    static error::BridgeResult<TruthSnapshot> read(
        const loader::AudioCoreFunctions& functions,
        kivo_audio_handle engine,
        bool truthCapabilitySupported = false);
};

} // namespace kivo::qt::audio_bridge::snapshot
