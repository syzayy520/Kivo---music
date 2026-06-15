// =============================================================================
// Kivo Music Qt - seek_command.cpp
// AudioCoreBridge: Seek command submission helper
// =============================================================================

#include "seek_command.hpp"

namespace kivo::qt::audio_bridge::command {

error::VoidResult SeekCommand::seekToFrame(
    const loader::AudioCoreFunctions& functions,
    kivo_audio_handle engine,
    uint64_t commandId,
    uint64_t sessionGeneration,
    uint64_t requestedFrame) {
    if (!functions.submitCommand) {
        return error::failure(error::BridgeError::commandFailed("seek", -1));
    }
    if (engine == KIVO_AUDIO_INVALID_HANDLE) {
        return error::failure(error::BridgeError::invalidState("Engine handle is invalid"));
    }

    kivo_audio_command_v1 command{};
    command.struct_size = sizeof(kivo_audio_command_v1);
    command.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    command.command_id = commandId;
    command.session_generation = sessionGeneration;
    command.kind = KIVO_AUDIO_COMMAND_SEEK;
    command.requested_frame = requestedFrame;

    const auto result = functions.submitCommand(engine, &command);
    if (result != KIVO_AUDIO_RESULT_OK) {
        return error::failure(error::BridgeError::commandFailed(
            "seek", static_cast<int>(result)));
    }
    return error::success();
}

} // namespace kivo::qt::audio_bridge::command
