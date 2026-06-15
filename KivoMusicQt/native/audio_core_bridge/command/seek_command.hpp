// =============================================================================
// Kivo Music Qt - seek_command.hpp
// AudioCoreBridge: Seek command submission helper
// =============================================================================

#pragma once

#include "../error/bridge_error.hpp"
#include "../loader/audio_core_function_resolver.hpp"

namespace kivo::qt::audio_bridge::command {

class SeekCommand final {
public:
    static error::VoidResult seekToFrame(
        const loader::AudioCoreFunctions& functions,
        kivo_audio_handle engine,
        uint64_t commandId,
        uint64_t sessionGeneration,
        uint64_t requestedFrame);
};

} // namespace kivo::qt::audio_bridge::command
