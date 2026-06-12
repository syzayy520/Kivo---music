#include "../fixture/fake_renderer_test_runner.hpp"
#include "../fixture/fake_renderer_test_fixture.hpp"

#include "kivo/testing/render/renderer/fake_renderer.hpp"

namespace {

using namespace kivo;

void lifecycle_open_start_stop_reset_close() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);

    FAKE_ASSERT(renderer.snapshot().state == core::render::RenderLifecycleState::Closed);
    FAKE_ASSERT(renderer.open(fake_renderer_test::open_request(config)).is_accepted());
    FAKE_ASSERT(renderer.start().is_success());
    FAKE_ASSERT(renderer.snapshot().state == core::render::RenderLifecycleState::Started);
    FAKE_ASSERT(renderer.stop().is_success());
    FAKE_ASSERT(renderer.snapshot().state == core::render::RenderLifecycleState::Stopped);

    core::render::RenderResetRequest reset{};
    reset.generations = renderer.snapshot().generations;
    FAKE_ASSERT(renderer.reset(reset).is_success());
    FAKE_ASSERT(renderer.snapshot().state == core::render::RenderLifecycleState::Open);
    FAKE_ASSERT(renderer.close().is_success());
    FAKE_ASSERT(renderer.snapshot().is_valid());
    FAKE_ASSERT(renderer.snapshot().state == core::render::RenderLifecycleState::Closed);
}

void write_before_start_is_rejected() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    FAKE_ASSERT(renderer.open(fake_renderer_test::open_request(config)).is_accepted());

    const auto result = renderer.write({});
    FAKE_ASSERT(result.disposition() == core::render::RenderWriteDisposition::Rejected);
    FAKE_ASSERT(result.failure() == core::render::RenderFailure::InvalidState);
}

void invalid_and_unsupported_formats_are_distinct() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);

    core::render::RenderOpenRequest invalid{};
    const auto invalid_result = renderer.open(invalid);
    FAKE_ASSERT(invalid_result.failure() == core::render::RenderFailure::InvalidFormat);

    auto unsupported = fake_renderer_test::open_request(config);
    unsupported.requested_format.format.sample_rate = 44100;
    const auto unsupported_result = renderer.open(unsupported);
    FAKE_ASSERT(unsupported_result.failure() == core::render::RenderFailure::UnsupportedFormat);
}

void compatible_policy_accepts_configured_format() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);

    auto request = fake_renderer_test::open_request(config);
    request.requested_format.format.sample_rate = 44100;
    request.format_policy = core::render::FormatAcceptancePolicy::AllowCompatible;
    const auto result = renderer.open(request);
    FAKE_ASSERT(result.is_accepted());
    FAKE_ASSERT(result.accepted_format() == config.supported_format);
}

void stale_device_generation_is_rejected_during_open() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);

    auto request = fake_renderer_test::open_request(config);
    request.generations = fake_renderer_test::next_device(request.generations);
    const auto result = renderer.open(request);
    FAKE_ASSERT(!result.is_accepted());
    FAKE_ASSERT(result.failure() == core::render::RenderFailure::StaleDeviceGeneration);
}

} // namespace

void run_fake_renderer_lifecycle_tests(FakeRendererTestRunner& runner) {
    runner.run("lifecycle_open_start_stop_reset_close", lifecycle_open_start_stop_reset_close);
    runner.run("write_before_start_is_rejected", write_before_start_is_rejected);
    runner.run(
        "invalid_and_unsupported_formats_are_distinct",
        invalid_and_unsupported_formats_are_distinct);
    runner.run(
        "compatible_policy_accepts_configured_format",
        compatible_policy_accepts_configured_format);
    runner.run(
        "stale_device_generation_is_rejected_during_open",
        stale_device_generation_is_rejected_during_open);
}
