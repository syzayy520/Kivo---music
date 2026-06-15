// =============================================================================
// Kivo Music Qt - playback_command.cpp
// AudioCoreBridge: Playback command submission helpers
// =============================================================================

#include "playback_command.hpp"

namespace kivo::qt::audio_bridge::command {

error::VoidResult PlaybackCommand::play(
    const loader::AudioCoreFunctions& functions,
    kivo_audio_handle engine,
    uint64_t commandId,
    uint64_t sessionGeneration) {
    return submit(functions, engine, commandId, sessionGeneration,
        KIVO_AUDIO_COMMAND_RESUME, "play");
}

error::VoidResult PlaybackCommand::pause(
    const loader::AudioCoreFunctions& functions,
    kivo_audio_handle engine,
    uint64_t commandId,
    uint64_t sessionGeneration) {
    return submit(functions, engine, commandId, sessionGeneration,
        KIVO_AUDIO_COMMAND_PAUSE, "pause");
}

error::VoidResult PlaybackCommand::stop(
    const loader::AudioCoreFunctions& functions,
    kivo_audio_handle engine,
    uint64_t commandId,
    uint64_t sessionGeneration) {
    return submit(functions, engine, commandId, sessionGeneration,
        KIVO_AUDIO_COMMAND_STOP, "stop");
}

error::VoidResult PlaybackCommand::submit(
    const loader::AudioCoreFunctions& functions,
    kivo_audio_handle engine,
    uint64_t commandId,
    uint64_t sessionGeneration,
    uint32_t kind,
    const char* label) {
    if (!functions.submitCommand) {
        return error::failure(error::BridgeError::commandFailed(label, -1));
    }
    if (engine == KIVO_AUDIO_INVALID_HANDLE) {
        return error::failure(error::BridgeError::invalidState("Engine handle is invalid"));
    }

    kivo_audio_command_v1 command{};
    command.struct_size = sizeof(kivo_audio_command_v1);
    command.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    command.command_id = commandId;
    command.session_generation = sessionGeneration;
    command.kind = kind;

    const auto result = functions.submitCommand(engine, &command);
    if (result != KIVO_AUDIO_RESULT_OK) {
        return error::failure(error::BridgeError::commandFailed(
            label, static_cast<int>(result)));
    }
    return error::success();
}

} // namespace kivo::qt::audio_bridge::command
