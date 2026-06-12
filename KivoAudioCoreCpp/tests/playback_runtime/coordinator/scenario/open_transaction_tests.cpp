#include "../fixture/runtime_test_assert.hpp"
#include "../fixture/runtime_test_decoder.hpp"
#include "../fixture/runtime_test_factory.hpp"
#include "../runner/playback_runtime_test_cases.hpp"

namespace playback_runtime_test {

void open_failure_rolls_back_every_boundary() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    ScriptedDecoder decoder;
    decoder.fail_open = true;
    testing::time::ManualClock clock;
    const auto configuration = renderer_configuration();
    testing::render::FakeRenderer renderer{configuration, clock};
    core::playback::PlaybackRuntimeCoordinator runtime{
        session,
        decoder,
        renderer};

    const auto result = runtime.open(
        command(1, core::contract::CommandKind::OpenSource, 1),
        source(11, 2),
        open_request(configuration, 3));

    RUNTIME_ASSERT(!result.succeeded());
    RUNTIME_ASSERT(
        result.failure
        == core::playback::PlaybackRuntimeFailure::DecodeOpenFailed);
    RUNTIME_ASSERT(decoder.close_count == 1);
    RUNTIME_ASSERT(
        renderer.snapshot().state
        == core::render::RenderLifecycleState::Closed);
    RUNTIME_ASSERT(
        session.snapshot().state == core::contract::CoreState::Idle);
    RUNTIME_ASSERT(runtime.snapshot().rolled_back_opens == 1);
}

} // namespace playback_runtime_test
