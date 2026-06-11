#pragma once

#include "../../generation_id.hpp"

namespace kivo::core::contract {

/// @brief Stream generation identity wrapper.
///
/// Tracks the generation of a decoded audio stream.
struct StreamGeneration {
    GenerationId id{GenerationId::initial()};
    
    [[nodiscard]] constexpr bool operator==(const StreamGeneration&) const noexcept = default;
    
    [[nodiscard]] constexpr uint64_t value() const noexcept { return id.value(); }
};

} // namespace kivo::core::contract