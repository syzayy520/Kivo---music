// =============================================================================
// Kivo Music Qt - bridge_session_clock.hpp
// AudioCoreBridge: Monotonic command and session ids
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::qt::audio_bridge::state {

class BridgeSessionClock final {
public:
    [[nodiscard]] uint64_t sessionGeneration() const noexcept;
    [[nodiscard]] uint64_t sourceGeneration() const noexcept;
    [[nodiscard]] uint64_t nextCommandId() noexcept;
    [[nodiscard]] uint64_t nextSessionGeneration() noexcept;
    [[nodiscard]] uint64_t nextSourceGeneration() noexcept;

private:
    uint64_t commandId_ = 0;
    uint64_t sessionGeneration_ = 1;
    uint64_t sourceGeneration_ = 1;
};

} // namespace kivo::qt::audio_bridge::state
