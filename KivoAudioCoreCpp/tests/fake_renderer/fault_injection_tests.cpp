#include "fake_renderer_tests_main.hpp"
#include "fake_renderer_test_fixture.hpp"

#include <array>
#include <chrono>
#include <cstddef>

namespace {

using namespace std::chrono_literals;
using namespace kivo;

void open_and_start(
    testing::render::FakeRenderer& renderer,
    const testing::render::FakeRendererConfiguration& config) {
    FAKE_ASSERT(renderer.open(fake_renderer_test::open_request(config)).is_accepted());
    FAKE_ASSERT(renderer.start().is_success());
}

void scheduled_partial_write_triggers_by_write_count() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);

    FAKE_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::PartialWrite,
        testing::render::RenderFaultTrigger::WriteCount,
        1,
        32
    }));
    std::array<std::byte, 128 * 8> bytes{};
    const auto result = renderer.write(
        fake_renderer_test::block(bytes, 128, 0, renderer.snapshot().generations));
    FAKE_ASSERT(result.disposition() == core::render::RenderWriteDisposition::Partial);
    FAKE_ASSERT(result.accepted_frames() == 32);
    FAKE_ASSERT(result.unwritten_ownership() == core::render::UnwrittenFrameOwnership::Caller);
}

void scheduled_partial_write_triggers_by_frame_position() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);

    FAKE_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::PartialWrite,
        testing::render::RenderFaultTrigger::FramePositionAtOrAfter,
        100,
        8
    }));
    std::array<std::byte, 16 * 8> bytes{};
    const auto result = renderer.write(
        fake_renderer_test::block(bytes, 16, 100, renderer.snapshot().generations));
    FAKE_ASSERT(result.accepted_frames() == 8);
}

void scheduled_fault_triggers_by_stream_generation() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);
    const auto generations = renderer.snapshot().generations;

    FAKE_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::PartialWrite,
        testing::render::RenderFaultTrigger::StreamGeneration,
        generations.stream.value(),
        4
    }));
    std::array<std::byte, 8 * 8> bytes{};
    const auto result =
        renderer.write(fake_renderer_test::block(bytes, 8, 0, generations));
    FAKE_ASSERT(result.accepted_frames() == 4);
}

void write_count_fault_survives_an_earlier_rejected_write() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);
    const auto generations = renderer.snapshot().generations;
    FAKE_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::PartialWrite,
        testing::render::RenderFaultTrigger::WriteCount,
        1,
        2
    }));

    std::array<std::byte, 8 * 8> bytes{};
    const auto stale = renderer.write(fake_renderer_test::block(
        bytes,
        8,
        0,
        fake_renderer_test::next_seek(generations)));
    FAKE_ASSERT(stale.failure() == core::render::RenderFailure::StaleSeekGeneration);

    const auto valid =
        renderer.write(fake_renderer_test::block(bytes, 8, 0, generations));
    FAKE_ASSERT(valid.disposition() == core::render::RenderWriteDisposition::Partial);
    FAKE_ASSERT(valid.accepted_frames() == 2);
}

void device_lost_during_write_is_terminal_until_reset_or_close() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);

    FAKE_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::DeviceLost,
        testing::render::RenderFaultTrigger::NextWrite
    }));
    std::array<std::byte, 8> bytes{};
    const auto result = renderer.write(
        fake_renderer_test::block(bytes, 1, 0, renderer.snapshot().generations));
    FAKE_ASSERT(result.disposition() == core::render::RenderWriteDisposition::Failed);
    FAKE_ASSERT(result.failure() == core::render::RenderFailure::DeviceLost);
    FAKE_ASSERT(renderer.snapshot().state == core::render::RenderLifecycleState::Failed);
    FAKE_ASSERT(renderer.snapshot().last_failure == core::render::RenderFailure::DeviceLost);
}

void forced_underrun_is_observed_without_sleep() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);

    std::array<std::byte, 64 * 8> bytes{};
    FAKE_ASSERT(renderer.write(
        fake_renderer_test::block(bytes, 64, 0, renderer.snapshot().generations))
        .disposition() == core::render::RenderWriteDisposition::Full);
    FAKE_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::ForcedUnderrun,
        testing::render::RenderFaultTrigger::NextConsume
    }));

    const auto result = renderer.consume(32);
    FAKE_ASSERT(result.underrun);
    FAKE_ASSERT(result.consumed_frames == 32);
    FAKE_ASSERT(renderer.snapshot().last_failure == core::render::RenderFailure::Underrun);
    FAKE_ASSERT(clock.now().time_since_epoch() > 0ns);
}

void drain_timeout_and_success_are_deterministic() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);

    std::array<std::byte, 240 * 8> bytes{};
    FAKE_ASSERT(renderer.write(
        fake_renderer_test::block(bytes, 240, 0, renderer.snapshot().generations))
        .disposition() == core::render::RenderWriteDisposition::Full);
    FAKE_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::DrainTimeout,
        testing::render::RenderFaultTrigger::NextDrain
    }));

    const auto timed_out = renderer.drain({1s});
    FAKE_ASSERT(timed_out.disposition() == core::render::RenderControlDisposition::TimedOut);
    FAKE_ASSERT(renderer.snapshot().buffered_frames == 240);
    FAKE_ASSERT(clock.now().time_since_epoch() == 0ns);

    const auto insufficient = renderer.drain({4ms});
    FAKE_ASSERT(insufficient.disposition() == core::render::RenderControlDisposition::TimedOut);
    FAKE_ASSERT(renderer.snapshot().buffered_frames == 240);
    FAKE_ASSERT(clock.now().time_since_epoch() == 0ns);

    const auto drained = renderer.drain({5ms});
    FAKE_ASSERT(drained.is_success());
    FAKE_ASSERT(renderer.snapshot().buffered_frames == 0);
    FAKE_ASSERT(renderer.snapshot().state == core::render::RenderLifecycleState::Stopped);
    FAKE_ASSERT(clock.now().time_since_epoch() == 5ms);
}

} // namespace

void run_fake_renderer_fault_injection_tests(FakeRendererTestRunner& runner) {
    runner.run(
        "scheduled_partial_write_triggers_by_write_count",
        scheduled_partial_write_triggers_by_write_count);
    runner.run(
        "scheduled_partial_write_triggers_by_frame_position",
        scheduled_partial_write_triggers_by_frame_position);
    runner.run(
        "scheduled_fault_triggers_by_stream_generation",
        scheduled_fault_triggers_by_stream_generation);
    runner.run(
        "write_count_fault_survives_an_earlier_rejected_write",
        write_count_fault_survives_an_earlier_rejected_write);
    runner.run(
        "device_lost_during_write_is_terminal_until_reset_or_close",
        device_lost_during_write_is_terminal_until_reset_or_close);
    runner.run(
        "forced_underrun_is_observed_without_sleep",
        forced_underrun_is_observed_without_sleep);
    runner.run(
        "drain_timeout_and_success_are_deterministic",
        drain_timeout_and_success_are_deterministic);
}
