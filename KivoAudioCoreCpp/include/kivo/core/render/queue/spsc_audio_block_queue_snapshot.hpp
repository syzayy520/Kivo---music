#pragma once

#include <cstddef>
#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"

namespace kivo::core::render {

struct SpscAudioBlockQueueSnapshot {
    uint32_t capacity_slots{0};
    uint32_t used_slots{0};
    uint32_t observed_peak_used_slots{0};
    size_t maximum_bytes_per_slot{0};
    contract::FrameCount queued_frames{0};
    uint64_t pushed_blocks{0};
    uint64_t consumed_blocks{0};
    uint64_t rejected_full{0};
    uint64_t rejected_oversized{0};
    bool closed{false};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return capacity_slots != 0
            && used_slots <= capacity_slots
            && observed_peak_used_slots <= capacity_slots
            && used_slots <= observed_peak_used_slots
            && maximum_bytes_per_slot != 0
            && consumed_blocks <= pushed_blocks;
    }

    [[nodiscard]] constexpr bool operator==(
        const SpscAudioBlockQueueSnapshot&) const noexcept = default;
};

} // namespace kivo::core::render
