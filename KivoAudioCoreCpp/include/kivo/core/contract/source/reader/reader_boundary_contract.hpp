#pragma once

#include "reader_identity.hpp"
#include "reader_capability.hpp"
#include "../access/source_ownership_policy.hpp"

namespace kivo::core::contract {

/// @brief Reader boundary contract — aggregated reader contract.
struct ReaderBoundaryContract {
    ReaderIdentity id{};
    ReaderCapability capability{};
    SourceOwnershipPolicy ownership{SourceOwnershipPolicy::NotSpecified};

    [[nodiscard]] bool operator==(const ReaderBoundaryContract&) const noexcept = default;
};

} // namespace kivo::core::contract
