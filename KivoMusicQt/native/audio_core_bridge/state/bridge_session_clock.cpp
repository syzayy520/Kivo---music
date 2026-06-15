// =============================================================================
// Kivo Music Qt - bridge_session_clock.cpp
// AudioCoreBridge: Monotonic command and session ids
// =============================================================================

#include "bridge_session_clock.hpp"

namespace kivo::qt::audio_bridge::state {

uint64_t BridgeSessionClock::sessionGeneration() const noexcept {
    return sessionGeneration_;
}

uint64_t BridgeSessionClock::sourceGeneration() const noexcept {
    return sourceGeneration_;
}

uint64_t BridgeSessionClock::nextCommandId() noexcept {
    return ++commandId_;
}

uint64_t BridgeSessionClock::nextSessionGeneration() noexcept {
    return ++sessionGeneration_;
}

uint64_t BridgeSessionClock::nextSourceGeneration() noexcept {
    return ++sourceGeneration_;
}

} // namespace kivo::qt::audio_bridge::state
