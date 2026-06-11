#include "fake_renderer_tests_main.hpp"
#include "fake_renderer_test_fixture.hpp"

#include <array>
#include <cstddef>

namespace {

using namespace kivo;

void open_and_start(
    testing::render::FakeRenderer& renderer,
    const testing::render::FakeRendererConfiguration& config) {
    FAKE_ASSERT(renderer.open(fake_renderer_test::open_request(config)).is_accepted());
    FAKE_ASSERT(renderer.start().is_success());
}

void normal_write_is_full_and_capacity_write_is_partial() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);

    std::array<std::byte, 64 * 8> first_bytes{};
    const auto generations = renderer.snapshot().generations;
    const auto first = renderer.write(
        fake_renderer_test::block(first_bytes, 64, 0, generations));
    FAKE_ASSERT(first.disposition() == core::render::RenderWriteDisposition::Full);

    std::array<std::byte, 300 * 8> second_bytes{};
    const auto second = renderer.write(
        fake_renderer_test::block(second_bytes, 300, 64, generations));
    FAKE_ASSERT(second.disposition() == core::render::RenderWriteDisposition::Partial);
    FAKE_ASSERT(second.accepted_frames() == 256);
    FAKE_ASSERT(second.unwritten_frames() == 44);
    FAKE_ASSERT(renderer.snapshot().buffered_frames == 320);
}

void each_stale_generation_is_rejected() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);
    const auto current = renderer.snapshot().generations;
    std::array<std::byte, 8> bytes{};

    const auto stream = renderer.write(
        fake_renderer_test::block(bytes, 1, 0, fake_renderer_test::next_stream(current)));
    FAKE_ASSERT(stream.failure() == core::render::RenderFailure::StaleStreamGeneration);

    const auto seek = renderer.write(
        fake_renderer_test::block(bytes, 1, 0, fake_renderer_test::next_seek(current)));
    FAKE_ASSERT(seek.failure() == core::render::RenderFailure::StaleSeekGeneration);

    const auto flush = renderer.write(
        fake_renderer_test::block(bytes, 1, 0, fake_renderer_test::next_flush(current)));
    FAKE_ASSERT(flush.failure() == core::render::RenderFailure::StaleFlushGeneration);

    const auto device = renderer.write(
        fake_renderer_test::block(bytes, 1, 0, fake_renderer_test::next_device(current)));
    FAKE_ASSERT(device.failure() == core::render::RenderFailure::StaleDeviceGeneration);
    FAKE_ASSERT(renderer.snapshot().buffered_frames == 0);
}

void flush_adopts_new_generations_and_rejects_old_frames() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);
    const auto old_generations = renderer.snapshot().generations;
    const auto new_generations = fake_renderer_test::next_flush(
        fake_renderer_test::next_seek(old_generations));

    FAKE_ASSERT(renderer.flush({new_generations}).is_success());
    std::array<std::byte, 8> bytes{};
    const auto stale =
        renderer.write(fake_renderer_test::block(bytes, 1, 0, old_generations));
    FAKE_ASSERT(stale.failure() == core::render::RenderFailure::StaleSeekGeneration);

    const auto current =
        renderer.write(fake_renderer_test::block(bytes, 1, 0, new_generations));
    FAKE_ASSERT(current.disposition() == core::render::RenderWriteDisposition::Full);
}

void full_capacity_delays_without_accepting_frames() {
    testing::time::ManualClock clock;
    const auto config = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer(config, clock);
    open_and_start(renderer, config);
    const auto generations = renderer.snapshot().generations;
    std::array<std::byte, 256 * 8> bytes{};

    for (int write = 0; write < 4; ++write) {
        FAKE_ASSERT(renderer.write(
            fake_renderer_test::block(bytes, 256, write * 256, generations))
            .disposition() == core::render::RenderWriteDisposition::Full);
    }
    const auto delayed = renderer.write(
        fake_renderer_test::block(bytes, 256, 1024, generations));
    FAKE_ASSERT(delayed.disposition() == core::render::RenderWriteDisposition::Delayed);
    FAKE_ASSERT(delayed.accepted_frames() == 0);
    FAKE_ASSERT(delayed.unwritten_frames() == 256);
}

} // namespace

void run_fake_renderer_write_generation_tests(FakeRendererTestRunner& runner) {
    runner.run(
        "normal_write_is_full_and_capacity_write_is_partial",
        normal_write_is_full_and_capacity_write_is_partial);
    runner.run("each_stale_generation_is_rejected", each_stale_generation_is_rejected);
    runner.run(
        "flush_adopts_new_generations_and_rejects_old_frames",
        flush_adopts_new_generations_and_rejects_old_frames);
    runner.run(
        "full_capacity_delays_without_accepting_frames",
        full_capacity_delays_without_accepting_frames);
}
