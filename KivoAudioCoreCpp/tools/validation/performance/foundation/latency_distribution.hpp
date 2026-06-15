#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace kivo::validation::performance {

struct LatencyDistribution {
    uint64_t minimum{0};
    uint64_t p50{0};
    uint64_t p95{0};
    uint64_t maximum{0};
};

[[nodiscard]] inline LatencyDistribution calculate_distribution(
    std::vector<uint64_t> values) {
    if (values.empty()) {
        throw std::invalid_argument{"latency distribution requires samples"};
    }
    std::sort(values.begin(), values.end());
    const auto nearest_rank = [&values](uint32_t percentile) {
        const auto numerator =
            static_cast<uint64_t>(percentile) * values.size();
        const auto rank = (numerator + 99) / 100;
        const auto index = rank == 0 ? size_t{0} : rank - 1;
        return values[index];
    };
    return {
        values.front(),
        nearest_rank(50),
        nearest_rank(95),
        values.back()
    };
}

} // namespace kivo::validation::performance
