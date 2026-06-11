// =============================================================================
// KivoAudioCoreCpp — timeline_range.hpp
// Contract: Timeline range in samples
// =============================================================================

#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// TimelineRange — Timeline range in samples
// =============================================================================
struct TimelineRange {
    uint64_t start_sample{0};
    uint64_t end_sample{0};

    [[nodiscard]] constexpr bool operator==(const TimelineRange&) const noexcept = default;
};

} // namespace kivo::core::contract
