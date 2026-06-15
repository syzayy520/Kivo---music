// =============================================================================
// Kivo Music Qt - engine_handle.hpp
// AudioCoreBridge: RAII wrapper for kivo_audio_handle
// =============================================================================

#pragma once

#include "../loader/audio_core_function_resolver.hpp"
#include "../error/bridge_error.hpp"

namespace kivo::qt::audio_bridge::handle {

// RAII wrapper for kivo_audio_handle
// Automatically destroys engine on destruction
class EngineHandle final {
public:
    // Create engine using the provided function table
    static error::BridgeResult<EngineHandle> create(
        const loader::AudioCoreFunctions& functions);

    ~EngineHandle();

    // Non-copyable, movable
    EngineHandle(const EngineHandle&) = delete;
    EngineHandle& operator=(const EngineHandle&) = delete;
    EngineHandle(EngineHandle&&) noexcept;
    EngineHandle& operator=(EngineHandle&&) noexcept;

    // Get raw handle (for passing to Host ABI)
    kivo_audio_handle raw() const { return handle_; }

    // Check if valid
    bool isValid() const { return handle_ != KIVO_AUDIO_INVALID_HANDLE; }

private:
    EngineHandle(kivo_audio_handle handle,
                 const loader::AudioCoreFunctions& functions);

    kivo_audio_handle handle_ = KIVO_AUDIO_INVALID_HANDLE;
    const loader::AudioCoreFunctions* functions_;
};

} // namespace kivo::qt::audio_bridge::handle

