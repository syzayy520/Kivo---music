#pragma once

#include "kivo/core/output/truth/evidence/bit_perfect_evidence.hpp"
#include "kivo/core/output/truth/result/bit_perfect_evaluation_result.hpp"

namespace kivo::core::output {

[[nodiscard]] BitPerfectEvaluationResult evaluate_bit_perfect_truth(
    const BitPerfectEvidence& evidence) noexcept;

} // namespace kivo::core::output
