#include "../fixture/runtime_test_assert.hpp"
#include "../fixture/runtime_test_decoder.hpp"
#include "../fixture/runtime_test_factory.hpp"
#include "../runner/playback_runtime_test_cases.hpp"

namespace playback_runtime_test {

void device_loss_during_flush_recovers_pipeline() {
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
        command(1, core::contract::CommandKind::OpenSource, 41),
        source(61, 42),
        open_request(configuration, 43)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(2, core::contract::CommandKind::Resume, 41)).succeeded());
    RUNTIME_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::DeviceLost,
        testing::render::RenderFaultTrigger::NextFlush
    }));

    const auto failed = runtime.execute(
        command(3, core::contract::CommandKind::Flush, 41));
    RUNTIME_ASSERT(!failed.succeeded());
    RUNTIME_ASSERT(
        failed.render_failure == core::render::RenderFailure::DeviceLost);
    RUNTIME_ASSERT(
        session.snapshot().state == core::contract::CoreState::Recovering);
    RUNTIME_ASSERT(runtime.snapshot().device_recovery_pending);

    RUNTIME_ASSERT(runtime.recover().succeeded());
    const auto recovered = runtime.snapshot();
    RUNTIME_ASSERT(
        recovered.session.state == core::contract::CoreState::Playing);
    RUNTIME_ASSERT(
        recovered.renderer.state
        == core::render::RenderLifecycleState::Started);
    RUNTIME_ASSERT(recovered.device_loss_events == 1);
    RUNTIME_ASSERT(recovered.successful_device_recoveries == 1);
}

} // namespace playback_runtime_test
