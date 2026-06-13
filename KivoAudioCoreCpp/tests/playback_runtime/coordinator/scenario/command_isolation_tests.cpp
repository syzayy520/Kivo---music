#include "../fixture/runtime_test_assert.hpp"
#include "../fixture/runtime_test_decoder.hpp"
#include "../fixture/runtime_test_factory.hpp"
#include "../runner/playback_runtime_test_cases.hpp"

namespace playback_runtime_test {

void stale_command_cannot_mutate_renderer() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    ScriptedDecoder decoder;
    testing::time::ManualClock clock;
    const auto configuration = renderer_configuration();
    testing::render::FakeRenderer renderer{configuration, clock};
    core::playback::PlaybackRuntimeCoordinator runtime{
        session,
        decoder,
        renderer};

    RUNTIME_ASSERT(runtime.open(
        command(1, core::contract::CommandKind::OpenSource, 7),
        source(13, 8),
        open_request(configuration, 9)).succeeded());
    const auto before = renderer.snapshot();
    const auto stale = runtime.execute(
        command(2, core::contract::CommandKind::Resume, 6));

    RUNTIME_ASSERT(!stale.succeeded());
    RUNTIME_ASSERT(
        stale.failure
        == core::playback::PlaybackRuntimeFailure::CommandRejected);
    RUNTIME_ASSERT(renderer.snapshot() == before);
    RUNTIME_ASSERT(
        session.snapshot().state == core::contract::CoreState::Ready);
}

} // namespace playback_runtime_test
