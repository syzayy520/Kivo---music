#include "../fixture/runtime_test_assert.hpp"
#include "../fixture/runtime_test_decoder.hpp"
#include "../fixture/runtime_test_factory.hpp"
#include "../runner/playback_runtime_test_cases.hpp"

namespace playback_runtime_test {

namespace {

void trigger_device_loss(
    kivo::core::playback::PlaybackRuntimeCoordinator& runtime,
    kivo::testing::render::FakeRenderer& renderer) {
    using namespace kivo;
    RUNTIME_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::DeviceLost,
        testing::render::RenderFaultTrigger::NextWrite
    }));
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);
    const auto failed = runtime.render_step();
    RUNTIME_ASSERT(
        failed.disposition == core::render::RenderPumpDisposition::Failed);
    RUNTIME_ASSERT(
        failed.failure == core::render::RenderFailure::DeviceLost);
}

} // namespace

void device_loss_reopens_pipeline_and_resumes() {
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
        source(31, 2),
        open_request(configuration, 3)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(2, core::contract::CommandKind::Resume, 1)).succeeded());
    RUNTIME_ASSERT(session.report_rendered_position(100, 1));
    trigger_device_loss(runtime, renderer);
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
    RUNTIME_ASSERT(decoder.seek_count == 1);
    RUNTIME_ASSERT(decoder.last_seek_target == 100);
    RUNTIME_ASSERT(recovered.decode_generation.value() == 4);
    RUNTIME_ASSERT(recovered.render_generations.seek.value() == 1);
    RUNTIME_ASSERT(recovered.render_generations.flush.value() == 1);
    RUNTIME_ASSERT(recovered.device_loss_events == 1);
    RUNTIME_ASSERT(recovered.device_recovery_attempts == 1);
    RUNTIME_ASSERT(recovered.successful_device_recoveries == 1);
    RUNTIME_ASSERT(recovered.failed_device_recoveries == 0);

    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);
    RUNTIME_ASSERT(
        runtime.render_step().disposition
        == core::render::RenderPumpDisposition::EndOfStream);
    RUNTIME_ASSERT(session.snapshot().rendered_position == 104);
}

void failed_device_recovery_remains_closable() {
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
        source(32, 6),
        open_request(configuration, 7)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(2, core::contract::CommandKind::Resume, 5)).succeeded());
    trigger_device_loss(runtime, renderer);
    decoder.fail_seek = true;

    const auto recovery = runtime.recover();
    const auto failed = runtime.snapshot();
    RUNTIME_ASSERT(!recovery.succeeded());
    RUNTIME_ASSERT(
        recovery.failure
        == core::playback::PlaybackRuntimeFailure::DecodeControlFailed);
    RUNTIME_ASSERT(
        recovery.decode_failure
        == core::decode::DecodeFailure::SourceSeekFailed);
    RUNTIME_ASSERT(
        failed.session.state == core::contract::CoreState::Failed);
    RUNTIME_ASSERT(
        failed.session.last_error_domain
        == core::contract::ErrorDomain::SourceUnavailable);
    RUNTIME_ASSERT(!failed.device_recovery_pending);
    RUNTIME_ASSERT(failed.failed_device_recoveries == 1);
    RUNTIME_ASSERT(
        renderer.snapshot().state
        == core::render::RenderLifecycleState::Closed);
    RUNTIME_ASSERT(
        runtime.recover().failure
        == core::playback::PlaybackRuntimeFailure::NoPendingRecovery);

    RUNTIME_ASSERT(runtime.execute(
        command(3, core::contract::CommandKind::CloseSource, 5)).succeeded());
    RUNTIME_ASSERT(!runtime.snapshot().active);
    RUNTIME_ASSERT(decoder.close_count == 1);
}

} // namespace playback_runtime_test
