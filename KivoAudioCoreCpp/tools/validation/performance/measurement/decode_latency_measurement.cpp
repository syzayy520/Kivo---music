#include "decode_latency_measurement.hpp"

#include <chrono>
#include <stdexcept>

#include "kivo/adapters/ffmpeg/session/ffmpeg_audio_decode_session.hpp"
#include "kivo/source/local/local_file_byte_source.hpp"

namespace kivo::validation::performance {

namespace {

using Clock = std::chrono::steady_clock;

[[nodiscard]] constexpr core::contract::RenderFormat target_format() noexcept {
    using namespace core::contract;
    return {{
        SampleFormat::Float32,
        ChannelLayout::Stereo,
        FrameLayout::Interleaved,
        {},
        48000,
        32,
        32
    }};
}

[[nodiscard]] constexpr core::contract::GenerationId generation(
    uint64_t value) noexcept {
    auto result = core::contract::GenerationId::initial();
    for (uint64_t index = 0; index < value; ++index) {
        result = core::contract::GenerationId::next(result);
    }
    return result;
}

[[nodiscard]] constexpr core::decode::DecodeOpenRequest open_request() noexcept {
    return {
        target_format(),
        {generation(11)},
        true
    };
}

[[nodiscard]] uint64_t elapsed_ns(Clock::time_point begin) noexcept {
    return static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            Clock::now() - begin).count());
}

[[nodiscard]] std::unique_ptr<core::decode::ByteSourceBoundary> open_source(
    const std::filesystem::path& path) {
    return source::local::LocalFileByteSource::open(
        path,
        {701},
        {generation(7)});
}

void require(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error{message};
    }
}

[[nodiscard]] DecodeLatencySample measure_once(
    const std::filesystem::path& fixture_path) {
    DecodeLatencySample sample{};
    adapters::ffmpeg::FfmpegAudioDecodeSession control_session;

    const auto open_begin = Clock::now();
    auto control_source = open_source(fixture_path);
    require(control_source != nullptr, "performance source open failed");
    const auto opened =
        control_session.open(std::move(control_source), open_request());
    sample.open_ns = elapsed_ns(open_begin);
    require(opened.is_accepted(), "performance decoder open failed");

    const auto first_begin = Clock::now();
    const auto first = control_session.decode_next();
    sample.first_block_ns = elapsed_ns(first_begin);
    require(
        first.disposition()
            == core::decode::DecodeStepDisposition::Produced,
        "performance first decode produced no block");

    const auto seek_begin = Clock::now();
    const auto seek = control_session.seek(24000, {generation(12)});
    sample.seek_ns = elapsed_ns(seek_begin);
    require(seek.is_success(), "performance seek failed");
    require(control_session.close().is_success(), "performance close failed");

    adapters::ffmpeg::FfmpegAudioDecodeSession decode_session;
    auto decode_source = open_source(fixture_path);
    require(decode_source != nullptr, "performance decode source open failed");
    require(
        decode_session.open(std::move(decode_source), open_request())
            .is_accepted(),
        "performance full decode open failed");

    const auto decode_begin = Clock::now();
    for (;;) {
        const auto step = decode_session.decode_next();
        if (step.disposition()
            == core::decode::DecodeStepDisposition::EndOfStream) {
            break;
        }
        require(
            step.disposition()
                == core::decode::DecodeStepDisposition::Produced,
            "performance full decode failed");
        sample.decoded_frames += step.block().frame_count;
        ++sample.decoded_blocks;
    }
    sample.full_decode_ns = elapsed_ns(decode_begin);
    require(sample.decoded_frames != 0, "performance decoded no frames");
    require(sample.decoded_blocks != 0, "performance decoded no blocks");
    require(decode_session.close().is_success(), "performance decode close failed");
    return sample;
}

} // namespace

FixturePerformanceEvidence measure_decode_latency(
    const std::filesystem::path& fixture_path,
    uint32_t iterations) {
    if (iterations == 0) {
        throw std::invalid_argument{"performance iterations must be positive"};
    }
    FixturePerformanceEvidence result{};
    result.fixture_name = fixture_path.filename().string();
    result.samples.reserve(iterations);
    static_cast<void>(measure_once(fixture_path));
    for (uint32_t index = 0; index < iterations; ++index) {
        result.samples.push_back(measure_once(fixture_path));
    }
    return result;
}

} // namespace kivo::validation::performance
