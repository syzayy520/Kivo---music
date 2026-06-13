#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace kivo::validation::performance {

struct DecodeLatencySample {
    uint64_t open_ns{0};
    uint64_t first_block_ns{0};
    uint64_t seek_ns{0};
    uint64_t full_decode_ns{0};
    uint64_t decoded_frames{0};
    uint64_t decoded_blocks{0};
};

struct FixturePerformanceEvidence {
    std::string fixture_name{};
    std::vector<DecodeLatencySample> samples{};
};

struct QueueTimingEvidence {
    uint32_t capacity_slots{0};
    uint32_t observed_peak_used_slots{0};
    uint64_t successful_pushes{0};
    uint64_t successful_consumes{0};
    std::vector<uint32_t> occupancy_samples{};
    std::vector<uint64_t> consumer_interval_ns{};
};

struct PerformanceProbeEvidence {
    uint32_t schema_version{1};
    uint32_t iterations{0};
    std::vector<FixturePerformanceEvidence> fixtures{};
    QueueTimingEvidence queue{};
};

} // namespace kivo::validation::performance
