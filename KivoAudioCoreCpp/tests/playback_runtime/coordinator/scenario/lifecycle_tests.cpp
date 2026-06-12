#include "../fixture/runtime_test_assert.hpp"
#include "../fixture/runtime_test_decoder.hpp"
#include "../fixture/runtime_test_factory.hpp"
#include "../runner/playback_runtime_test_cases.hpp"

namespace playback_runtime_test {

void failed_seek_can_be_closed_and_shutdown_is_terminal() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    ScriptedDecoder decoder;
    decoder.fail_seek = true;
    testing::time::ManualClock clock;
    const auto configuration = renderer_configuration();
    testing::render::FakeRenderer renderer{configuration, clock};
    core::playback::PlaybackRuntimeCoordinator runtime{
        session,
        decoder,
        renderer};

    RUNTIME_ASSERT(runtime.open(
        command(1, core::contract::CommandKind::OpenSource, 3),
        source(14, 4),
        open_request(configuration, 5)).succeeded());
    const auto seek_result = runtime.seek(
        command(2, core::contract::CommandKind::Seek, 3, 200));
    RUNTIME_ASSERT(!seek_result.succeeded());
    RUNTIME_ASSERT(
        session.snapshot().state == core::contract::CoreState::Failed);
    RUNTIME_ASSERT(runtime.execute(
        command(3, core::contract::CommandKind::CloseSource, 3)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(4, core::contract::CommandKind::Shutdown, 0)).succeeded());
    RUNTIME_ASSERT(
        session.snapshot().state == core::contract::CoreState::Closed);
    RUNTIME_ASSERT(
        runtime.execute(
            command(5, core::contract::CommandKind::Shutdown, 0)).failure
        == core::playback::PlaybackRuntimeFailure::RuntimeClosed);
}

} // namespace playback_runtime_test
