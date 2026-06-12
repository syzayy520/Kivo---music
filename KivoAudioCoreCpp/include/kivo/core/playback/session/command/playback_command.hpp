#pragma once

#include "kivo/core/contract/command/identity/command_generation_ref.hpp"
#include "kivo/core/contract/command/identity/command_id.hpp"
#include "kivo/core/contract/command/kind/command_kind.hpp"
#include "kivo/core/contract/sample_position.hpp"

namespace kivo::core::playback {

struct PlaybackCommand {
    contract::CommandId id{};
    contract::CommandKind kind{contract::CommandKind::Unknown};
    contract::CommandGenerationRef generation{};
    contract::SamplePosition requested_position{
        contract::kInvalidSamplePosition};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return id.value != 0
            && kind != contract::CommandKind::Unknown
            && generation.origin != contract::CommandOrigin::Unknown;
    }

    [[nodiscard]] constexpr bool operator==(
        const PlaybackCommand&) const noexcept = default;
};

} // namespace kivo::core::playback
