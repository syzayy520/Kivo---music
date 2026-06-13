#include "../fixture/runtime_test_assert.hpp"
#include "../fixture/runtime_test_decoder.hpp"
#include "../fixture/runtime_test_factory.hpp"
#include "../runner/playback_runtime_test_cases.hpp"

namespace playback_runtime_test {

void source_failure_stops_media_with_classified_domain() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    ScriptedDecoder decoder;
    decoder.terminal_failure =
        core::decode::DecodeFailure::SourceReadFailed;
    testing::time::ManualClock clock;
    const auto configuration = renderer_configuration();
    testing::render::FakeRenderer renderer{configuration, clock};
    core::playback::PlaybackRuntimeCoordinator runtime{
        session,
        decoder,
        renderer};

    RUNTIME_ASSERT(runtime.open(
        command(1, core::contract::CommandKind::OpenSource, 7),
        source(41, 8),
        open_request(configuration, 9)).succeeded());
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Failed);

    const auto failed = runtime.snapshot();
    RUNTIME_ASSERT(
        failed.session.state == core::contract::CoreState::Stopped);
    RUNTIME_ASSERT(
        failed.session.last_error_domain
        == core::contract::ErrorDomain::SourceUnavailable);
    RUNTIME_ASSERT(
        failed.renderer.state
        == core::render::RenderLifecycleState::Stopped);
    RUNTIME_ASSERT(failed.active);
    RUNTIME_ASSERT(failed.decode_failure_events == 1);
    RUNTIME_ASSERT(failed.recoverable_decode_failures == 1);
    RUNTIME_ASSERT(failed.decode_fallback_stops == 1);
    RUNTIME_ASSERT(failed.failed_decode_recoveries == 0);
    RUNTIME_ASSERT(
        failed.last_decode_failure
        == core::decode::DecodeFailure::SourceReadFailed);
    RUNTIME_ASSERT(
        failed.decode_failures_by_category[
            core::observability::decode_failure_category_index(
                core::observability::DecodeFailureCategory::Source)]
        == 1);

    RUNTIME_ASSERT(runtime.execute(
        command(3, core::contract::CommandKind::CloseSource, 7)).succeeded());
}

void codec_failure_is_truthful_and_remains_closable() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    ScriptedDecoder decoder;
    decoder.terminal_failure =
        core::decode::DecodeFailure::UnsupportedCodec;
    testing::time::ManualClock clock;
    const auto configuration = renderer_configuration();
    testing::render::FakeRenderer renderer{configuration, clock};
    core::playback::PlaybackRuntimeCoordinator runtime{
        session,
        decoder,
        renderer};

    RUNTIME_ASSERT(runtime.open(
        command(1, core::contract::CommandKind::OpenSource, 11),
        source(42, 12),
        open_request(configuration, 13)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(2, core::contract::CommandKind::Resume, 11)).succeeded());
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Failed);

    const auto failed = runtime.snapshot();
    RUNTIME_ASSERT(
        failed.session.state == core::contract::CoreState::Failed);
    RUNTIME_ASSERT(
        failed.session.last_error_domain
        == core::contract::ErrorDomain::CodecError);
    RUNTIME_ASSERT(failed.session.recovery_attempts == 1);
    RUNTIME_ASSERT(failed.session.recovery_failures == 1);
    RUNTIME_ASSERT(failed.decode_failure_events == 1);
    RUNTIME_ASSERT(failed.recoverable_decode_failures == 1);
    RUNTIME_ASSERT(failed.decode_fallback_stops == 0);
    RUNTIME_ASSERT(failed.failed_decode_recoveries == 1);
    RUNTIME_ASSERT(
        failed.decode_failures_by_category[
            core::observability::decode_failure_category_index(
                core::observability::DecodeFailureCategory::Codec)]
        == 1);

    RUNTIME_ASSERT(runtime.execute(
        command(3, core::contract::CommandKind::CloseSource, 11)).succeeded());
    RUNTIME_ASSERT(!runtime.snapshot().active);
}

} // namespace playback_runtime_test
