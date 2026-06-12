#pragma once

#include <cstdint>

namespace kivo::core::playback {

struct PlaybackDrainTicket {
    uint64_t session_generation{0};
    uint64_t drain_generation{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return session_generation != 0 && drain_generation != 0;
    }

    [[nodiscard]] constexpr bool operator==(
        const PlaybackDrainTicket&) const noexcept = default;
};

} // namespace kivo::core::playback
