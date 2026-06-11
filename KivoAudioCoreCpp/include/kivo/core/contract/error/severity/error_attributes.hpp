#pragma once

#include <cstdint>

#include "error_severity.hpp"
#include "safe_cleanup_path.hpp"
#include "../recovery/recovery_action.hpp"

namespace kivo::core::contract {

struct ErrorAttributes {
    ErrorSeverity severity{ErrorSeverity::Unknown};
    bool recoverable{false};
    bool retryable{false};
    bool requires_reopen_device{false};
    bool requires_rebuild_codec{false};
    bool requires_user_action{false};
    bool safe_to_continue{false};
    RecoveryAction recovery_action{RecoveryAction::None};
    bool host_notification{false};
    bool metrics_event{false};
    SafeCleanupPath safe_cleanup_path{SafeCleanupPath::None};

    [[nodiscard]] bool operator==(const ErrorAttributes&) const noexcept = default;
};

} // namespace kivo::core::contract
