#pragma once

#include "../../generation_id.hpp"

namespace kivo::core::contract {

/// @brief Source generation identity wrapper.
///
/// Tracks the generation of a source boundary state.
struct SourceGeneration {
    GenerationId id{GenerationId::initial()};

    [[nodiscard]] constexpr uint64_t value() const noexcept { return id.value(); }

    [[nodiscard]] constexpr bool operator==(const SourceGeneration&) const noexcept = default;
};

} // namespace kivo::core::contract
