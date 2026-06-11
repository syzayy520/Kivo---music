#pragma once

#include "command_ordering_policy.hpp"
#include "command_supersession_policy.hpp"
#include "command_coalescing_policy.hpp"

namespace kivo::core::contract {

struct CommandOrderingContract {
    CommandOrderingPolicy ordering{CommandOrderingPolicy::Unknown};
    CommandSupersessionPolicy supersession{CommandSupersessionPolicy::Unknown};
    CommandCoalescingPolicy coalescing{CommandCoalescingPolicy::Unknown};
    [[nodiscard]] bool operator==(const CommandOrderingContract&) const noexcept = default;
};

} // namespace kivo::core::contract