#pragma once

#include <filesystem>

#include "../foundation/performance_probe_model.hpp"

namespace kivo::validation::performance {

void write_performance_evidence(
    const std::filesystem::path& output_path,
    const PerformanceProbeEvidence& evidence);

} // namespace kivo::validation::performance
