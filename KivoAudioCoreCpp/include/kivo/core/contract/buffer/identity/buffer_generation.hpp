#pragma once

#include "../../generation_id.hpp"

namespace kivo::core::contract {

struct BufferGeneration {
    GenerationId id{GenerationId::initial()};

    [[nodiscard]] constexpr bool operator==(const BufferGeneration&) const noexcept = default;
    [[nodiscard]] constexpr uint64_t value() const noexcept { return id.value(); }
};

} // namespace kivo::core::contract