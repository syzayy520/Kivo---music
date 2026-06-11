#pragma once
#include <cstdint>

namespace kivo::core::contract::policy {

struct BinaryProvenancePolicy {
    uint8_t require_build_signature{0};
    uint8_t require_reproducible_build{0};
    uint8_t require_source_hash{0};

    [[nodiscard]] constexpr bool operator==(const BinaryProvenancePolicy&) const noexcept = default;
};

} // namespace kivo::core::contract::policy
