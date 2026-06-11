#pragma once

#include <cstdint>

#include "kivo/core/contract/command/identity/command_id.hpp"
#include "kivo/core/contract/error/domain/error_domain.hpp"
#include "kivo/core/contract/error/recovery/recovery_action.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/contract/state/phase/core_state.hpp"
#include "kivo/core/playback/session/playback_command_failure.hpp"

namespace kivo::core::playback {

struct PlaybackSessionSnapshot {
    contract::CoreState state{contract::CoreState::Idle};
    uint64_t session_generation{0};
    contract::CommandId last_command_id{};
    contract::SamplePosition rendered_position{0};
    contract::SamplePosition paused_position{0};
    contract::SamplePosition pending_seek_target{
        contract::kInvalidSamplePosition};
    uint64_t seek_generation{0};
    uint64_t flush_generation{0};
    uint64_t accepted_commands{0};
    uint64_t rejected_commands{0};
    uint64_t stale_commands{0};
    uint64_t coalesced_commands{0};
    uint64_t source_replacements{0};
    uint64_t recovery_attempts{0};
    uint64_t recovery_successes{0};
    uint64_t recovery_failures{0};
    contract::ErrorDomain last_error_domain{
        contract::ErrorDomain::Unknown};
    contract::RecoveryAction recovery_action{
        contract::RecoveryAction::None};
    PlaybackCommandFailure last_failure{PlaybackCommandFailure::None};

    [[nodiscard]] constexpr bool operator==(
        const PlaybackSessionSnapshot&) const noexcept = default;
};

} // namespace kivo::core::playback
