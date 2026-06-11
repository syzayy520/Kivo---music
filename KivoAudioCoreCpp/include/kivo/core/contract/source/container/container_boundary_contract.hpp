#pragma once

#include "container_identity.hpp"
#include "container_capability.hpp"
#include "../access/source_ownership_policy.hpp"

namespace kivo::core::contract {

/// @brief Container boundary contract — aggregated container contract.
struct ContainerBoundaryContract {
    ContainerIdentity id{};
    ContainerCapability capability{};
    SourceOwnershipPolicy ownership{SourceOwnershipPolicy::NotSpecified};

    [[nodiscard]] constexpr bool operator==(const ContainerBoundaryContract&) const noexcept = default;
};

} // namespace kivo::core::contract
