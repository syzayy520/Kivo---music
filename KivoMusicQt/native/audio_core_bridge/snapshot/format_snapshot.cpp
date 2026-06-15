// =============================================================================
// Kivo Music Qt - format_snapshot.cpp
// AudioCoreBridge: Format text projection from Host ABI snapshot
// =============================================================================

#include "format_snapshot.hpp"

namespace kivo::qt::audio_bridge::snapshot {

QString FormatSnapshot::formatText() const {
    return text;
}

error::BridgeResult<FormatSnapshot> FormatSnapshotReader::read(
    const loader::AudioCoreFunctions& functions,
    kivo_audio_handle engine) {
    if (!functions.getSnapshot) {
        return error::BridgeResult<FormatSnapshot>::failed(
            error::BridgeError{error::BridgeErrorCategory::Snapshot,
                "getSnapshot function not available", 0});
    }
    if (engine == KIVO_AUDIO_INVALID_HANDLE) {
        return error::BridgeResult<FormatSnapshot>::failed(
            error::BridgeError::invalidState("Engine handle is invalid"));
    }

    kivo_audio_snapshot_v1 snapshot{};
    snapshot.struct_size = sizeof(kivo_audio_snapshot_v1);
    snapshot.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    const auto result = functions.getSnapshot(engine, &snapshot);
    if (result != KIVO_AUDIO_RESULT_OK) {
        return error::BridgeResult<FormatSnapshot>::failed(
            error::BridgeError{error::BridgeErrorCategory::Snapshot,
                "Failed to read snapshot", static_cast<int>(result)});
    }

    FormatSnapshot format{};
    if (snapshot.active != 0u) {
        format.text = "Playing";
    } else if (snapshot.successful_opens > 0u) {
        format.text = "Ready";
    } else {
        format.text = "Choose music";
    }
    return error::BridgeResult<FormatSnapshot>::ok(format);
}

} // namespace kivo::qt::audio_bridge::snapshot
