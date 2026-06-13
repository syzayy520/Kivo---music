#include "../fixture/stability_decode_factory.hpp"
#include "../fixture/stability_test_runner.hpp"
#include "../platform/windows/process_resource_probe.hpp"
#include "../runner/stability_test_cases.hpp"

#include <array>
#include <iostream>

namespace {

using namespace kivo;
using namespace stability_test;

void run_decode_cycles(
    const StabilityTestContext& context,
    uint32_t cycle_count,
    uint64_t identity_base) {
    constexpr std::array<const char*, 3> fixture_names{
        "tone_pcm_s16le.wav",
        "tone.flac",
        "tone.mp3"
    };

    for (uint32_t cycle = 0; cycle < cycle_count; ++cycle) {
        adapters::ffmpeg::FfmpegAudioDecodeSession session;
        const auto identity = identity_base + cycle + 1;
        auto source = open_source(
            context.fixture_directory
                / fixture_names[cycle % fixture_names.size()],
            identity);
        STABILITY_ASSERT(source != nullptr);
        STABILITY_ASSERT(
            session.open(
                std::move(source),
                open_request(identity)).is_accepted());

        uint32_t block_count = 0;
        for (;;) {
            const auto step = session.decode_next();
            if (step.disposition()
                == core::decode::DecodeStepDisposition::EndOfStream) {
                break;
            }
            STABILITY_ASSERT(
                step.disposition()
                == core::decode::DecodeStepDisposition::Produced);
            STABILITY_ASSERT(step.block().is_valid());
            ++block_count;
            STABILITY_ASSERT(block_count < 10000);
        }
        STABILITY_ASSERT(block_count > 0);
        STABILITY_ASSERT(session.close().is_success());
        STABILITY_ASSERT(!session.probe().is_valid());
    }
}

} // namespace

void repeated_ffmpeg_lifecycle_has_bounded_resource_growth(
    const StabilityTestContext& context) {
    constexpr uint32_t warmup_cycles = 6;
    constexpr uint32_t measured_cycles_per_phase = 24;
    constexpr uint64_t maximum_total_private_growth = 8ull * 1024 * 1024;
    constexpr uint64_t maximum_second_phase_private_growth = 4ull * 1024 * 1024;
    constexpr uint32_t maximum_handle_growth = 2;

    run_decode_cycles(context, warmup_cycles, 1000);
    const auto baseline = stability_test::capture_process_resources();
    STABILITY_ASSERT(baseline.has_value());

    run_decode_cycles(context, measured_cycles_per_phase, 2000);
    const auto first = stability_test::capture_process_resources();
    STABILITY_ASSERT(first.has_value());

    run_decode_cycles(context, measured_cycles_per_phase, 3000);
    const auto second = stability_test::capture_process_resources();
    STABILITY_ASSERT(second.has_value());

    const auto total_private_growth = stability_test::positive_growth(
        baseline->private_bytes,
        second->private_bytes);
    const auto second_phase_private_growth = stability_test::positive_growth(
        first->private_bytes,
        second->private_bytes);

    std::cout
        << "\n    handles=" << baseline->handle_count
        << "->" << first->handle_count
        << "->" << second->handle_count
        << " private_bytes=" << baseline->private_bytes
        << "->" << first->private_bytes
        << "->" << second->private_bytes
        << "\n    ";

    STABILITY_ASSERT(
        second->handle_count
        <= baseline->handle_count + maximum_handle_growth);
    STABILITY_ASSERT(total_private_growth <= maximum_total_private_growth);
    STABILITY_ASSERT(
        second_phase_private_growth
        <= maximum_second_phase_private_growth);
}
