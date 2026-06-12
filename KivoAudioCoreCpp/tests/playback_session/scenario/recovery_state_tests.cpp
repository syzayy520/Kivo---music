#include "kivo/core/playback/session/controller/playback_session_controller.hpp"

#include "../fixture/session_test_assert.hpp"
#include "../fixture/session_test_factory.hpp"
#include "../runner/playback_session_test_cases.hpp"

namespace playback_session_test {

void recovery_policy_is_classified_and_generation_scoped() {
    using namespace kivo;
    using core::contract::ErrorDomain;
    using core::contract::RecoveryAction;
    constexpr auto device =
        core::playback::classify_recovery(ErrorDomain::DeviceLost);
    constexpr auto source_decision =
        core::playback::classify_recovery(ErrorDomain::SourceUnavailable);
    constexpr auto unsupported =
        core::playback::classify_recovery(ErrorDomain::UnsupportedFormat);
    static_assert(device.recoverable);
    static_assert(device.primary == RecoveryAction::ReopenDevice);
    static_assert(source_decision.primary == RecoveryAction::RebuildSource);
    static_assert(!unsupported.recoverable);

    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    const auto stale_decision =
        session.begin_recovery(9, ErrorDomain::DeviceLost);
    SESSION_ASSERT(!stale_decision.recoverable);
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Playing);
    const auto decision =
        session.begin_recovery(10, ErrorDomain::DeviceLost);
    SESSION_ASSERT(decision.recoverable);
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Recovering);
    SESSION_ASSERT(!session.complete_recovery(9, true));
    SESSION_ASSERT(session.complete_recovery(10, true));
    auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Playing);
    SESSION_ASSERT(snapshot.recovery_attempts == 1);
    SESSION_ASSERT(snapshot.recovery_successes == 1);

    static_cast<void>(
        session.begin_recovery(10, ErrorDomain::SourceUnavailable));
    SESSION_ASSERT(session.complete_recovery(10, false));
    snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Failed);
    SESSION_ASSERT(snapshot.recovery_failures == 1);
}

void recovery_interrupts_seek_without_restoring_stale_intent() {
    using namespace kivo;
    using core::contract::ErrorDomain;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        3, core::contract::CommandKind::Seek, 10, 48000)).accepted());
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Seeking);

    const auto decision =
        session.begin_recovery(10, ErrorDomain::DeviceLost);
    SESSION_ASSERT(decision.recoverable);
    auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Recovering);
    SESSION_ASSERT(
        snapshot.pending_seek_target
        == core::contract::kInvalidSamplePosition);
    SESSION_ASSERT(!session.complete_seek(10, true));
    SESSION_ASSERT(session.complete_recovery(10, true));
    snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Ready);
}

void new_source_clears_previous_error_identity() {
    using namespace kivo;
    using core::contract::ErrorDomain;
    using core::contract::RecoveryAction;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    static_cast<void>(
        session.begin_recovery(10, ErrorDomain::SourceUnavailable));
    SESSION_ASSERT(session.complete_recovery(10, false));
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::OpenSource, 11)).accepted());
    const auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.last_error_domain == ErrorDomain::Unknown);
    SESSION_ASSERT(snapshot.recovery_action == RecoveryAction::None);
}

} // namespace playback_session_test
