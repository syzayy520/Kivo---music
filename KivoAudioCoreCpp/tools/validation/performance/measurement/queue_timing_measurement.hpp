#pragma once

#include "../foundation/performance_probe_model.hpp"

namespace kivo::validation::performance {

[[nodiscard]] QueueTimingEvidence measure_queue_timing();

} // namespace kivo::validation::performance
