#include "fake_renderer_tests_main.hpp"
#include "fake_renderer_test_fixture.hpp"

#include <array>
#include <cstddef>
#include <type_traits>
#include <utility>

namespace {

using namespace kivo;

static_assert(std::is_base_of_v<
    core::render::RenderBoundary,
    testing::render::FakeRenderer>);
static_assert(noexcept(std::declval<testing::render::FakeRenderer&>().write(
    std::declval<const core::render::AudioBlockView&>())));

void bounded_history_keeps_latest_observations_in_order() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration(3);
    testing::render::FakeRenderer renderer(config, clock);

    FAKE_ASSERT(renderer.open(fake_renderer_test::open_request(config)).is_accepted());
    FAKE_ASSERT(renderer.start().is_success());
    FAKE_ASSERT(renderer.stop().is_success());
    FAKE_ASSERT(renderer.close().is_success());

    FAKE_ASSERT(renderer.observation_count() == 3);
    FAKE_ASSERT(
        renderer.observation_at(0)->operation
        == testing::render::RenderObservationOperation::Start);
    FAKE_ASSERT(
        renderer.observation_at(1)->operation
        == testing::render::RenderObservationOperation::Stop);
    FAKE_ASSERT(
        renderer.observation_at(2)->operation
        == testing::render::RenderObservationOperation::Close);
    FAKE_ASSERT(!renderer.observation_at(3).has_value());
}

void observation_history_can_be_cleared_without_reallocation() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration(4);
    testing::render::FakeRenderer renderer(config, clock);

    FAKE_ASSERT(renderer.open(fake_renderer_test::open_request(config)).is_accepted());
    FAKE_ASSERT(renderer.observation_count() == 1);
    renderer.clear_observations();
    FAKE_ASSERT(renderer.observation_count() == 0);
    FAKE_ASSERT(renderer.start().is_success());
    FAKE_ASSERT(renderer.observation_count() == 1);
}

void fault_table_is_bounded_and_reuses_consumed_slots() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    FAKE_ASSERT(renderer.open(fake_renderer_test::open_request(config)).is_accepted());
    FAKE_ASSERT(renderer.start().is_success());

    const testing::render::ScheduledRenderFault fault{
        testing::render::RenderFaultKind::PartialWrite,
        testing::render::RenderFaultTrigger::NextWrite,
        0,
        1
    };
    for (size_t index = 0; index < testing::render::FakeRenderer::kMaximumScheduledFaults; ++index) {
        FAKE_ASSERT(renderer.schedule_fault(fault));
    }
    FAKE_ASSERT(!renderer.schedule_fault(fault));

    std::array<std::byte, 16> bytes{};
    const auto result = renderer.write(
        fake_renderer_test::block(bytes, 2, 0, renderer.snapshot().generations));
    FAKE_ASSERT(result.accepted_frames() == 1);
    FAKE_ASSERT(renderer.schedule_fault(fault));
}

void oversized_observation_capacity_is_rejected_by_configuration() {
    auto config = fake_renderer_test::configuration(
        testing::render::kMaximumObservationHistoryCapacity + 1);
    FAKE_ASSERT(!config.is_valid());
}

void close_clears_faults_between_sessions() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    FAKE_ASSERT(renderer.open(fake_renderer_test::open_request(config)).is_accepted());
    FAKE_ASSERT(renderer.start().is_success());
    FAKE_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::DeviceLost,
        testing::render::RenderFaultTrigger::NextWrite
    }));
    FAKE_ASSERT(renderer.close().is_success());
    FAKE_ASSERT(renderer.open(fake_renderer_test::open_request(config)).is_accepted());
    FAKE_ASSERT(renderer.start().is_success());

    std::array<std::byte, 8> bytes{};
    const auto result = renderer.write(
        fake_renderer_test::block(bytes, 1, 0, renderer.snapshot().generations));
    FAKE_ASSERT(result.disposition() == core::render::RenderWriteDisposition::Full);
}

} // namespace

void run_fake_renderer_observation_history_tests(FakeRendererTestRunner& runner) {
    runner.run(
        "bounded_history_keeps_latest_observations_in_order",
        bounded_history_keeps_latest_observations_in_order);
    runner.run(
        "observation_history_can_be_cleared_without_reallocation",
        observation_history_can_be_cleared_without_reallocation);
    runner.run(
        "fault_table_is_bounded_and_reuses_consumed_slots",
        fault_table_is_bounded_and_reuses_consumed_slots);
    runner.run(
        "oversized_observation_capacity_is_rejected_by_configuration",
        oversized_observation_capacity_is_rejected_by_configuration);
    runner.run("close_clears_faults_between_sessions", close_clears_faults_between_sessions);
}
