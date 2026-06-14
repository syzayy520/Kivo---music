#pragma once

#include <cstdint>

#include "dop_marker.hpp"

namespace kivo::core::contract {

[[nodiscard]] constexpr uint8_t expected_dop_marker_for_frame(
    uint64_t pcm_frame_index,
    DopMarkerPair marker_pair = {}) noexcept {
    return (pcm_frame_index % 2u == 0u) ? marker_pair.first
                                        : marker_pair.second;
}

[[nodiscard]] constexpr bool dop_marker_matches_frame(
    uint64_t pcm_frame_index,
    uint8_t marker,
    DopMarkerPair marker_pair = {}) noexcept {
    return marker_pair.is_valid()
        && marker == expected_dop_marker_for_frame(pcm_frame_index,
                                                   marker_pair);
}

struct DopFrameMarkerSync {
    uint8_t first_channel_marker{0};
    uint8_t last_channel_marker{0};
    uint8_t channel_count{0};

    [[nodiscard]] constexpr bool channels_are_synchronized() const noexcept {
        return channel_count != 0
            && first_channel_marker == last_channel_marker;
    }

    [[nodiscard]] constexpr bool matches_frame(
        uint64_t pcm_frame_index,
        DopMarkerPair marker_pair = {}) const noexcept {
        return channels_are_synchronized()
            && dop_marker_matches_frame(pcm_frame_index,
                                        first_channel_marker,
                                        marker_pair);
    }

    [[nodiscard]] constexpr bool operator==(
        const DopFrameMarkerSync&) const noexcept = default;
};

} // namespace kivo::core::contract
