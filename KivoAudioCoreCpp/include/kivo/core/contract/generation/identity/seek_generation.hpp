#pragma once

#include "../../generation_id.hpp"

namespace kivo::core::contract {

/// @brief Seek generation identity wrapper.
///
/// Tracks the generation of a seek operation.
struct SeekGenerationId {
    GenerationId id{GenerationId::initial()};
    
    [[nodiscard]] constexpr bool operator==(const SeekGenerationId&) const noexcept = default;
    
    [[nodiscard]] constexpr uint64_t value() const noexcept { return id.value(); }
};

} // namespace kivo::core::contract