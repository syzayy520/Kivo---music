#pragma once

#include <cstdint>

#include "../domain/error_domain.hpp"
#include "recovery_action.hpp"

namespace kivo::core::contract {

struct RecoveryMatrix {
    ErrorDomain domain = ErrorDomain::Unknown;
    RecoveryAction primary_action = RecoveryAction::None;
    RecoveryAction fallback_action = RecoveryAction::None;

    [[nodiscard]] bool operator==(const RecoveryMatrix&) const noexcept = default;
};

} // namespace kivo::core::contract