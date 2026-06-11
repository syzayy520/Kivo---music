// =============================================================================
// KivoAudioCoreCpp — source_time_range.hpp
// Contract: Source time range in samples
// =============================================================================
//


#pragma once

#include <cstdint>

namespace kivo::core::contract {

// =============================================================================
// SourceTimeRange — Source time range in samples
// =============================================================================
struct SourceTimeRange {
    uint64_t source_ref{0};
    uint64_t start_sample{0};
    uint64_t end_sample{0};
    uint32_t sample_rate{0};

    [[nodiscard]] bool operator==(const SourceTimeRange&) const noexcept = default;
};

} // namespace kivo::core::contract
