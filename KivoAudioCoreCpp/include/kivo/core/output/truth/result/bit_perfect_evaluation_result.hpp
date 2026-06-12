#pragma once

#include "kivo/core/contract/output/truth/bitperfect_truth_report.hpp"

namespace kivo::core::output {

struct BitPerfectEvaluationResult {
    contract::BitPerfectTruthReport report{};

    [[nodiscard]] bool is_bit_perfect() const noexcept {
        return report.is_bit_perfect();
    }
};

} // namespace kivo::core::output
