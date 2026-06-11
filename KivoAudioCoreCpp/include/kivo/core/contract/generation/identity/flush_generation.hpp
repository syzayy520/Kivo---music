#pragma once

#include "../../generation_id.hpp"

namespace kivo::core::contract {

/// @brief Flush generation identity wrapper.
///
/// Tracks the generation of a flush operation.
struct FlushGeneration {
    GenerationId id{GenerationId::initial()};
    
    [[nodiscard]] constexpr bool operator==(const FlushGeneration&) const noexcept = default;
    
    [[nodiscard]] constexpr uint64_t value() const noexcept { return id.value(); }
};

} // namespace kivo::core::contract