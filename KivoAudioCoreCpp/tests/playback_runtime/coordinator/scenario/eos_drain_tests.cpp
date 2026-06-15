#include "../fixture/runtime_test_assert.hpp"
#include "../fixture/runtime_test_decoder.hpp"
#include "../fixture/runtime_test_factory.hpp"
#include "../runner/playback_runtime_test_cases.hpp"

namespace playback_runtime_test {

void eos_drains_buffer_and_stops_session() {
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
        command(1, core::contract::CommandKind::OpenSource, 1),
        source(21, 2),
        open_request(configuration, 3)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(2, core::contract::CommandKind::Resume, 1)).succeeded());
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);

    const auto eos = runtime.render_step();
    const auto snapshot = runtime.snapshot();
    RUNTIME_ASSERT(
        eos.disposition == core::render::RenderPumpDisposition::EndOfStream);
    RUNTIME_ASSERT(
        snapshot.session.state == core::contract::CoreState::Stopped);
    RUNTIME_ASSERT(
        snapshot.renderer.state
        == core::render::RenderLifecycleState::Stopped);
    RUNTIME_ASSERT(snapshot.renderer.buffered_frames == 0);
    RUNTIME_ASSERT(snapshot.session.rendered_position == 4);
    RUNTIME_ASSERT(snapshot.drain_attempts == 1);
    RUNTIME_ASSERT(snapshot.successful_drains == 1);
    RUNTIME_ASSERT(snapshot.failed_drains == 0);
    RUNTIME_ASSERT(clock.now().time_since_epoch().count() > 0);
    RUNTIME_ASSERT(
        runtime.render_step().disposition
        == core::render::RenderPumpDisposition::Rejected);
    RUNTIME_ASSERT(runtime.execute(
        command(3, core::contract::CommandKind::CloseSource, 1)).succeeded());
}

void drain_timeout_fails_and_close_releases_boundaries() {
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
        command(1, core::contract::CommandKind::OpenSource, 5),
        source(22, 6),
        open_request(configuration, 7)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(2, core::contract::CommandKind::Resume, 5)).succeeded());
    RUNTIME_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::DrainTimeout,
        testing::render::RenderFaultTrigger::NextDrain
    }));
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);

    const auto failed = runtime.render_step();
    const auto before_close = runtime.snapshot();
    RUNTIME_ASSERT(
        failed.disposition == core::render::RenderPumpDisposition::Failed);
    RUNTIME_ASSERT(failed.failure == core::render::RenderFailure::Timeout);
    RUNTIME_ASSERT(
        before_close.session.state == core::contract::CoreState::Failed);
    RUNTIME_ASSERT(
        before_close.renderer.state
        == core::render::RenderLifecycleState::Draining);
    RUNTIME_ASSERT(before_close.drain_attempts == 1);
    RUNTIME_ASSERT(before_close.failed_drains == 1);
    RUNTIME_ASSERT(before_close.drain_timeouts == 1);

    RUNTIME_ASSERT(runtime.execute(
        command(3, core::contract::CommandKind::CloseSource, 5)).succeeded());
    RUNTIME_ASSERT(!runtime.snapshot().active);
    RUNTIME_ASSERT(decoder.close_count == 1);
    RUNTIME_ASSERT(
        renderer.snapshot().state
        == core::render::RenderLifecycleState::Closed);
}

} // namespace playback_runtime_test
