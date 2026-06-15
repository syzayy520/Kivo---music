#pragma once

#include "kivo/core/contract/generation_id.hpp"

namespace kivo::core::decode {

struct DecodeGeneration {
    contract::GenerationId id{contract::GenerationId::initial()};

    [[nodiscard]] constexpr uint64_t value() const noexcept {
        return id.value();
    }

    [[nodiscard]] constexpr bool operator==(
        const DecodeGeneration&) const noexcept = default;
};

} // namespace kivo::core::decode
