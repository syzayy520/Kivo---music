#pragma once

#include "../../generation_id.hpp"

namespace kivo::core::contract {

/// @brief Device generation identity wrapper.
///
/// Tracks the generation of a device state change.
struct DeviceGeneration {
    GenerationId id{GenerationId::initial()};
    
    [[nodiscard]] constexpr bool operator==(const DeviceGeneration&) const noexcept = default;
    
    [[nodiscard]] constexpr uint64_t value() const noexcept { return id.value(); }
};

} // namespace kivo::core::contract