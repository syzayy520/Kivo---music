#pragma once

#include <cstdint>
#include <filesystem>

#include "../foundation/performance_probe_model.hpp"

namespace kivo::validation::performance {

[[nodiscard]] FixturePerformanceEvidence measure_decode_latency(
    const std::filesystem::path& fixture_path,
    uint32_t iterations);

} // namespace kivo::validation::performance
