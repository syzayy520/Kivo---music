#pragma once

#include "kivo/core/playback/session/command/playback_command.hpp"

namespace playback_session_test {

[[nodiscard]] constexpr kivo::core::playback::PlaybackCommand command(
    uint64_t id,
    kivo::core::contract::CommandKind kind,
    uint64_t generation,
    kivo::core::contract::SamplePosition position =
        kivo::core::contract::kInvalidSamplePosition) noexcept {
    return {
        {id},
        kind,
        {generation, kivo::core::contract::CommandOrigin::User},
        position
    };
}

} // namespace playback_session_test
