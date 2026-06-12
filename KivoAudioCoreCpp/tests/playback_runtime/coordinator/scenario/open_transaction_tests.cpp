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

void processing_policy_is_propagated_and_bit_perfect_conflicts_reject() {
    using namespace kivo;
    {
        core::playback::PlaybackSessionController session;
        ScriptedDecoder decoder;
        testing::time::ManualClock clock;
        const auto configuration = renderer_configuration();
        testing::render::FakeRenderer renderer{configuration, clock};
        core::playback::PlaybackRuntimeCoordinator runtime{
            session,
            decoder,
            renderer};
        auto request = open_request(configuration, 3);
        request.resample_quality =
            core::processing::ResampleQuality::High;
        request.conversion_dither =
            core::processing::DitherPolicy::TriangularPdf;
        RUNTIME_ASSERT(runtime.open(
            command(1, core::contract::CommandKind::OpenSource, 1),
            source(11, 2),
            request).succeeded());
        RUNTIME_ASSERT(
            decoder.last_resample_quality
            == core::processing::ResampleQuality::High);
        RUNTIME_ASSERT(
            decoder.last_conversion_dither
            == core::processing::DitherPolicy::TriangularPdf);
    }
    {
        const auto configuration = renderer_configuration();
        auto request = open_request(configuration, 3);
        request.allow_conversion = true;
        request.producer_configuration.processing.bit_perfect_required =
            true;
        RUNTIME_ASSERT(!request.is_valid());
        request.allow_conversion = false;
        request.conversion_dither =
            core::processing::DitherPolicy::TriangularPdf;
        RUNTIME_ASSERT(!request.is_valid());
    }
}

} // namespace playback_runtime_test
