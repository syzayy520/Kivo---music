#include <thread>
#include <vector>

#include "kivo/core/playback/session/controller/playback_session_controller.hpp"

#include "../fixture/session_test_assert.hpp"
#include "../fixture/session_test_factory.hpp"
#include "../runner/playback_session_test_cases.hpp"

namespace playback_session_test {

void lifecycle_and_pause_freeze() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.snapshot().state == core::contract::CoreState::Idle);
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());
    SESSION_ASSERT(session.report_rendered_position(48000, 10));
    SESSION_ASSERT(session.submit(command(
        3, core::contract::CommandKind::Pause, 10)).accepted());
    const auto paused = session.snapshot();
    SESSION_ASSERT(paused.state == core::contract::CoreState::Paused);
    SESSION_ASSERT(paused.paused_position == 48000);
    SESSION_ASSERT(!session.report_rendered_position(49000, 10));
    SESSION_ASSERT(session.snapshot().paused_position == 48000);
    SESSION_ASSERT(session.submit(command(
        4, core::contract::CommandKind::Resume, 10)).accepted());
    SESSION_ASSERT(session.report_rendered_position(50000, 10));
}

void replacement_rejects_old_session_commands() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    const auto replaced = session.submit(command(
        2, core::contract::CommandKind::OpenSource, 11));
    SESSION_ASSERT(
        replaced.decision == core::contract::CommandDecision::Replace);
    const auto stale = session.submit(command(
        3, core::contract::CommandKind::Pause, 10));
    SESSION_ASSERT(!stale.accepted());
    SESSION_ASSERT(
        stale.failure == core::playback::PlaybackCommandFailure::StaleSession);
    SESSION_ASSERT(session.snapshot().session_generation == 11);
    SESSION_ASSERT(session.snapshot().source_replacements == 1);
}

void command_ids_are_globally_monotonic() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        10, core::contract::CommandKind::OpenSource, 10)).accepted());
    const auto stale = session.submit(command(
        9, core::contract::CommandKind::Resume, 10));
    SESSION_ASSERT(!stale.accepted());
    SESSION_ASSERT(
        stale.failure
        == core::playback::PlaybackCommandFailure::OutOfOrderCommand);
    SESSION_ASSERT(session.snapshot().last_command_id.value == 10);
}

void failed_session_can_close_and_shutdown() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.report_failure(10));
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Failed);
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::CloseSource, 10)).accepted());
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Idle);
    SESSION_ASSERT(session.submit(command(
        3, core::contract::CommandKind::Shutdown, 0)).accepted());
    SESSION_ASSERT(
        session.snapshot().state == core::contract::CoreState::Closed);
    const auto rejected = session.submit(command(
        4, core::contract::CommandKind::OpenSource, 11));
    SESSION_ASSERT(
        rejected.failure
        == core::playback::PlaybackCommandFailure::SessionClosed);
}

void concurrent_intake_remains_serialized() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    SESSION_ASSERT(session.submit(command(
        1, core::contract::CommandKind::OpenSource, 10)).accepted());
    SESSION_ASSERT(session.submit(command(
        2, core::contract::CommandKind::Resume, 10)).accepted());

    std::vector<std::thread> threads;
    for (uint64_t id = 3; id <= 102; ++id) {
        threads.emplace_back([&session, id]() {
            const auto kind = (id % 2) == 0
                ? core::contract::CommandKind::Resume
                : core::contract::CommandKind::Pause;
            static_cast<void>(session.submit(command(id, kind, 10)));
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }
    const auto snapshot = session.snapshot();
    SESSION_ASSERT(snapshot.last_command_id.value == 102);
    SESSION_ASSERT(
        snapshot.accepted_commands + snapshot.rejected_commands == 102);
    SESSION_ASSERT(
        snapshot.state == core::contract::CoreState::Playing
        || snapshot.state == core::contract::CoreState::Paused);
}

} // namespace playback_session_test
