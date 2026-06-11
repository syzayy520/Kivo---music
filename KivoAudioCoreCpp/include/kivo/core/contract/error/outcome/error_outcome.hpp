#pragma once

#include "../domain/error_domain.hpp"
#include "../severity/error_attributes.hpp"
#include "../recovery/recovery_action.hpp"

namespace kivo::core::contract {

struct ErrorOutcome {
    ErrorDomain domain{ErrorDomain::Unknown};
    ErrorAttributes attributes{};
    RecoveryAction recommended_recovery{RecoveryAction::None};

    [[nodiscard]] constexpr bool operator==(const ErrorOutcome&) const noexcept = default;
};

} // namespace kivo::core::contract
