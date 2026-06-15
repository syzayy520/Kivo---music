// =============================================================================
// Kivo Music Qt - truth_snapshot.cpp
// AudioCoreBridge: Truth status projection from Host ABI snapshot
// =============================================================================

#include "truth_snapshot.hpp"

namespace kivo::qt::audio_bridge::snapshot {

QString TruthSnapshot::statusBadge() const {
    switch (bitPerfectStatus) {
    case BitPerfectStatus::CoreReady:
        return "Core Ready";
    case BitPerfectStatus::Active:
        return "Signal Active";
    case BitPerfectStatus::Failed:
        return "Audio Error";
    case BitPerfectStatus::Unknown:
    default:
        return "--";
    }
}

error::BridgeResult<TruthSnapshot> TruthSnapshotReader::read(
    const loader::AudioCoreFunctions& functions,
    kivo_audio_handle engine) {
    if (!functions.getSnapshot) {
        return error::BridgeResult<TruthSnapshot>::failed(
            error::BridgeError{error::BridgeErrorCategory::Snapshot,
                "getSnapshot function not available", 0});
    }
    if (engine == KIVO_AUDIO_INVALID_HANDLE) {
        return error::BridgeResult<TruthSnapshot>::failed(
            error::BridgeError::invalidState("Engine handle is invalid"));
    }

    kivo_audio_snapshot_v1 snapshot{};
    snapshot.struct_size = sizeof(kivo_audio_snapshot_v1);
    snapshot.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    const auto result = functions.getSnapshot(engine, &snapshot);
    if (result != KIVO_AUDIO_RESULT_OK) {
        return error::BridgeResult<TruthSnapshot>::failed(
            error::BridgeError{error::BridgeErrorCategory::Snapshot,
                "Failed to read snapshot", static_cast<int>(result)});
    }

    TruthSnapshot truth{};
    if (snapshot.state == KIVO_AUDIO_STATE_FAILED) {
        truth.bitPerfectStatus = BitPerfectStatus::Failed;
    } else if (snapshot.active != 0u) {
        truth.bitPerfectStatus = BitPerfectStatus::Active;
    } else {
        truth.bitPerfectStatus = BitPerfectStatus::CoreReady;
    }
    return error::BridgeResult<TruthSnapshot>::ok(truth);
}

} // namespace kivo::qt::audio_bridge::snapshot
