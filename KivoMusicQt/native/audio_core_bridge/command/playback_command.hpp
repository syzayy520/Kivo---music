// =============================================================================
// Kivo Music Qt - playback_command.hpp
// AudioCoreBridge: Playback command submission helpers
// =============================================================================

#pragma once

#include "../error/bridge_error.hpp"
#include "../loader/audio_core_function_resolver.hpp"

namespace kivo::qt::audio_bridge::command {

class PlaybackCommand final {
public:
    static error::VoidResult play(
        const loader::AudioCoreFunctions& functions,
        kivo_audio_handle engine,
        uint64_t commandId,
        uint64_t sessionGeneration);

    static error::VoidResult pause(
        const loader::AudioCoreFunctions& functions,
        kivo_audio_handle engine,
        uint64_t commandId,
        uint64_t sessionGeneration);

    static error::VoidResult stop(
        const loader::AudioCoreFunctions& functions,
        kivo_audio_handle engine,
        uint64_t commandId,
        uint64_t sessionGeneration);

private:
    static error::VoidResult submit(
        const loader::AudioCoreFunctions& functions,
        kivo_audio_handle engine,
        uint64_t commandId,
        uint64_t sessionGeneration,
        uint32_t kind,
        const char* label);
};

} // namespace kivo::qt::audio_bridge::command
