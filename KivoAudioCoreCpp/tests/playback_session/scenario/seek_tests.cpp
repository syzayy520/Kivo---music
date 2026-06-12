#include "kivo/core/playback/session/controller/playback_session_controller.hpp"

#include "../fixture/session_test_assert.hpp"
#include "../fixture/session_test_factory.hpp"
#include "../runner/playback_session_test_cases.hpp"

namespace playback_session_test {

void rapid_seek_coalesces_and_restores_state() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    SESSION_ASSERT(session.report_rendered_position(12000, 10));

    const auto first = session.submit(command(
        3, core::contract::CommandKind::Seek, 10, 24000));
    const auto second = session.submit(command(
        4, core::contract::CommandKind::Seek, 10, 36000));
    SESSION_ASSERT(
        first.decision == core::contract::CommandDecision::Accept);
    SESSION_ASSERT(
        second.decision == core::contract::CommandDecision::Coalesce);
    auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Seeking);
    SESSION_ASSERT(snapshot.pending_seek_target == 36000);
    SESSION_ASSERT(snapshot.seek_generation == 2);
    SESSION_ASSERT(snapshot.coalesced_commands == 1);
    SESSION_ASSERT(session.complete_seek(10, true));
    snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Playing);
    SESSION_ASSERT(snapshot.rendered_position == 36000);
}

void paused_seek_stays_paused_and_flush_advances() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    SESSION_ASSERT(session.report_rendered_position(12000, 10));
    SESSION_ASSERT(session.submit(command(
        3, core::contract::CommandKind::Pause, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        4, core::contract::CommandKind::Seek, 10, 6000)).accepted());
    SESSION_ASSERT(session.complete_seek(10, true));
    auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.state == core::contract::CoreState::Paused);
    SESSION_ASSERT(snapshot.rendered_position == 6000);
    SESSION_ASSERT(snapshot.paused_position == 6000);
    SESSION_ASSERT(session.submit(command(
        5, core::contract::CommandKind::Flush, 10)).accepted());
    snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.flush_generation == 1);
}

} // namespace playback_session_test
