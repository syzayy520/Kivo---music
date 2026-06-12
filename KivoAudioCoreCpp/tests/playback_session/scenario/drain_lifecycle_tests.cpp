#include "kivo/core/playback/session/controller/playback_session_controller.hpp"

#include "../fixture/session_test_assert.hpp"
#include "../fixture/session_test_factory.hpp"
#include "../runner/playback_session_test_cases.hpp"

namespace playback_session_test {

namespace {

void start_playing(
    kivo::core::playback::PlaybackSessionController& session,
    uint64_t generation,
    uint64_t first_command_id) {
    using namespace kivo;
    SESSION_ASSERT(session.submit(command(
        first_command_id,
        core::contract::CommandKind::OpenSource,
        generation)).accepted());
    SESSION_ASSERT(session.submit(command(
        first_command_id + 1,
        core::contract::CommandKind::Resume,
        generation)).accepted());
}

} // namespace

void drain_completion_is_generation_safe() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    start_playing(session, 10, 1);
    SESSION_ASSERT(session.report_rendered_position(48000, 10));

    const auto old_ticket = session.begin_drain(10);
    SESSION_ASSERT(old_ticket.is_valid());
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Draining);
    SESSION_ASSERT(session.submit(command(
        3, core::contract::CommandKind::CloseSource, 10)).accepted());

    start_playing(session, 11, 4);
    const auto current_ticket = session.begin_drain(11);
    SESSION_ASSERT(current_ticket.is_valid());
    SESSION_ASSERT(!session.complete_drain(
        old_ticket,
        core::playback::PlaybackDrainCompletion::Succeeded));
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Draining);
    SESSION_ASSERT(session.complete_drain(
        current_ticket,
        core::playback::PlaybackDrainCompletion::Succeeded));

    const auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Stopped);
    SESSION_ASSERT(snapshot.drain_attempts == 2);
    SESSION_ASSERT(snapshot.drain_successes == 1);
    SESSION_ASSERT(snapshot.drain_failures == 0);
    SESSION_ASSERT(snapshot.drain_cancellations == 1);
}

void drain_failure_enters_failed_state() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    start_playing(session, 20, 1);
    const auto ticket = session.begin_drain(20);
    SESSION_ASSERT(ticket.is_valid());
    SESSION_ASSERT(session.complete_drain(
        ticket,
        core::playback::PlaybackDrainCompletion::Failed));

    const auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Failed);
    SESSION_ASSERT(snapshot.drain_attempts == 1);
    SESSION_ASSERT(snapshot.drain_successes == 0);
    SESSION_ASSERT(snapshot.drain_failures == 1);
    SESSION_ASSERT(!session.complete_drain(
        ticket,
        core::playback::PlaybackDrainCompletion::Succeeded));
}

void drain_command_races_are_deterministic() {
    using namespace kivo;
    {
        core::playback::PlaybackSessionController session;
        start_playing(session, 30, 1);
        const auto ticket = session.begin_drain(30);
        const auto pause = session.submit(command(
            3, core::contract::CommandKind::Pause, 30));
        SESSION_ASSERT(!pause.accepted());
        SESSION_ASSERT(
            pause.failure
            == core::playback::PlaybackCommandFailure::IllegalState);
        SESSION_ASSERT(session.submit(command(
            4, core::contract::CommandKind::Stop, 30)).accepted());
        SESSION_ASSERT(
            session.snapshot().state == core::contract::CoreState::Stopped);
        SESSION_ASSERT(session.snapshot().drain_cancellations == 1);
        SESSION_ASSERT(!session.complete_drain(
            ticket,
            core::playback::PlaybackDrainCompletion::Succeeded));
    }
    {
        core::playback::PlaybackSessionController session;
        start_playing(session, 40, 1);
        const auto ticket = session.begin_drain(40);
        SESSION_ASSERT(session.submit(command(
            3, core::contract::CommandKind::Shutdown, 40)).accepted());
        SESSION_ASSERT(
            session.snapshot().state == core::contract::CoreState::Closed);
        SESSION_ASSERT(session.snapshot().drain_cancellations == 1);
        SESSION_ASSERT(!session.complete_drain(
            ticket,
            core::playback::PlaybackDrainCompletion::Failed));
    }
}

} // namespace playback_session_test
