#pragma once

#include "kivo/core/contract/error/domain/error_domain.hpp"
#include "kivo/core/contract/error/recovery/recovery_action.hpp"

namespace kivo::core::playback {

struct PlaybackRecoveryDecision {
    contract::ErrorDomain domain{contract::ErrorDomain::Unknown};
    contract::RecoveryAction primary{contract::RecoveryAction::None};
    contract::RecoveryAction fallback{contract::RecoveryAction::None};
    bool recoverable{false};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return domain != contract::ErrorDomain::Unknown
            && primary != contract::RecoveryAction::None;
    }

    [[nodiscard]] constexpr bool operator==(
        const PlaybackRecoveryDecision&) const noexcept = default;
};

[[nodiscard]] constexpr PlaybackRecoveryDecision classify_recovery(
    contract::ErrorDomain domain) noexcept {
    using contract::ErrorDomain;
    using contract::RecoveryAction;
    switch (domain) {
    case ErrorDomain::DeviceLost:
        return {domain, RecoveryAction::ReopenDevice,
                RecoveryAction::RebuildOutput, true};
    case ErrorDomain::FormatChanged:
        return {domain, RecoveryAction::ReconfigureFormat,
                RecoveryAction::ReopenDevice, true};
    case ErrorDomain::SourceUnavailable:
    case ErrorDomain::NetworkInterrupted:
        return {domain, RecoveryAction::RebuildSource,
                RecoveryAction::StopMedia, true};
    case ErrorDomain::CodecError:
    case ErrorDomain::CodecStarved:
        return {domain, RecoveryAction::RebuildCodec,
                RecoveryAction::SkipTrack, true};
    case ErrorDomain::BufferUnderrun:
    case ErrorDomain::BufferOverrun:
        return {domain, RecoveryAction::Flush,
                RecoveryAction::RebuildOutput, true};
    case ErrorDomain::UnsupportedFormat:
    case ErrorDomain::PermissionDenied:
    case ErrorDomain::MediaError:
        return {domain, RecoveryAction::ReportToUser,
                RecoveryAction::SkipTrack, false};
    case ErrorDomain::InternalBug:
        return {domain, RecoveryAction::StopMedia,
                RecoveryAction::ReportToUser, false};
    default:
        return {domain, RecoveryAction::StopMedia,
                RecoveryAction::ReportToUser, false};
    }
}

} // namespace kivo::core::playback
