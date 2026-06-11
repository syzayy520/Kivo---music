#pragma once

#include "../reader/reader_boundary_contract.hpp"
#include "../container/container_boundary_contract.hpp"
#include "../access/source_readiness.hpp"
#include "../access/source_fault_policy.hpp"

namespace kivo::core::contract {

/// @brief Source boundary contract — aggregated source-level boundary.
///
/// Combines reader and container boundaries with readiness and fault policy.
struct SourceBoundaryContract {
    ReaderBoundaryContract reader{};
    ContainerBoundaryContract container{};
    SourceReadiness readiness{SourceReadiness::NotSpecified};
    SourceFaultPolicy fault_policy{SourceFaultPolicy::NotSpecified};

    [[nodiscard]] constexpr bool operator==(const SourceBoundaryContract&) const noexcept = default;
};

} // namespace kivo::core::contract
