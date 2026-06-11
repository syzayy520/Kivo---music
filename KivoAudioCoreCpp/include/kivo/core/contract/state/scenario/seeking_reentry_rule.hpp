#pragma once

#include <cstdint>

#include "../phase/core_state.hpp"
#include "../rule/reentrant_transition_policy.hpp"

namespace kivo::core::contract {

struct SeekingReentryRule {
    CoreState active_state = CoreState::Seeking;
    ReentrantTransitionPolicy policy = ReentrantTransitionPolicy::Merge;

    [[nodiscard]] bool operator==(const SeekingReentryRule&) const noexcept = default;
};

} // namespace kivo::core::contract