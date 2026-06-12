#include "../fixture/decode_boundary_test_runner.hpp"

#include <array>
#include <cstddef>

#include "kivo/core/decode/media/media_probe.hpp"
#include "kivo/core/decode/result/decode_control_result.hpp"
#include "kivo/core/decode/result/decode_open_result.hpp"
#include "kivo/core/decode/result/decode_step_result.hpp"

namespace {

using namespace kivo;

[[nodiscard]] constexpr core::contract::RenderFormat output_format() noexcept {
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

[[nodiscard]] constexpr core::contract::NativeDecodedFormat native_format() noexcept {
    using namespace core::contract;
    return {{
        SampleFormat::Int16,
        ChannelLayout::Stereo,
        FrameLayout::Interleaved,
        {},
        44100,
        16,
        16
    }};
}

[[nodiscard]] constexpr core::decode::MediaProbe valid_probe() noexcept {
    core::decode::MediaProbe probe{};
    probe.source_identity = {42};
    probe.codec = core::decode::AudioCodec::Flac;
    probe.container = core::decode::MediaContainer::Flac;
    probe.native_format = native_format();
    probe.output_format = output_format();
    probe.conversion = {true, true, false, true};
    probe.conversion_snapshot.conversion = probe.conversion;
    probe.resample = {
        true,
        core::contract::ResampleReason::DeviceMismatch,
        44100,
        48000
    };
    probe.duration_frames = 441000;
    probe.duration_known = true;
    probe.seekable = true;
    return probe;
}

void media_probe_requires_complete_truth() {
    constexpr auto probe = valid_probe();
    static_assert(probe.is_valid());
    DECODE_ASSERT(probe.is_valid());

    auto invalid = probe;
    invalid.resample.target_rate = 44100;
    DECODE_ASSERT(!invalid.is_valid());
    invalid = probe;
    invalid.codec = core::decode::AudioCodec::Unknown;
    DECODE_ASSERT(!invalid.is_valid());
}

void open_result_rejects_invalid_probe() {
    const auto accepted = core::decode::DecodeOpenResult::accepted(valid_probe());
    DECODE_ASSERT(accepted.is_accepted());
    DECODE_ASSERT(accepted.failure() == core::decode::DecodeFailure::None);

    const auto invalid = core::decode::DecodeOpenResult::accepted({});
    DECODE_ASSERT(!invalid.is_accepted());
    DECODE_ASSERT(invalid.failure() == core::decode::DecodeFailure::BoundaryFailure);
}

void decoded_block_requires_exact_frame_bytes() {
    const std::array<std::byte, 32> bytes{};
    const core::decode::DecodedAudioBlockView valid{
        bytes,
        output_format(),
        4,
        100,
        {},
        {},
        true
    };
    DECODE_ASSERT(valid.is_valid());

    auto invalid = valid;
    invalid.frame_count = 5;
    DECODE_ASSERT(!invalid.is_valid());
}

void step_result_keeps_eos_distinct_from_failure() {
    const std::array<std::byte, 32> bytes{};
    const core::decode::DecodedAudioBlockView block{
        bytes,
        output_format(),
        4,
        0,
        {},
        {},
        false
    };
    const auto produced = core::decode::DecodeStepResult::produced(block);
    DECODE_ASSERT(
        produced.disposition() == core::decode::DecodeStepDisposition::Produced);
    DECODE_ASSERT(produced.failure() == core::decode::DecodeFailure::None);

    const auto eos = core::decode::DecodeStepResult::end_of_stream();
    DECODE_ASSERT(
        eos.disposition() == core::decode::DecodeStepDisposition::EndOfStream);
    DECODE_ASSERT(eos.failure() == core::decode::DecodeFailure::EndOfStream);

    const auto failed = core::decode::DecodeStepResult::failed(
        core::decode::DecodeFailure::SourceReadFailed);
    DECODE_ASSERT(
        failed.disposition() == core::decode::DecodeStepDisposition::Failed);
}

void unsupported_seek_and_flush_are_explicit() {
    const auto seek = core::decode::DecodeControlResult::unsupported(
        core::decode::DecodeFailure::SeekUnsupported);
    const auto flush = core::decode::DecodeControlResult::unsupported(
        core::decode::DecodeFailure::FlushUnsupported);
    DECODE_ASSERT(
        seek.disposition == core::decode::DecodeControlDisposition::Unsupported);
    DECODE_ASSERT(seek.failure == core::decode::DecodeFailure::SeekUnsupported);
    DECODE_ASSERT(flush.failure == core::decode::DecodeFailure::FlushUnsupported);
}

} // namespace

void run_decode_contract_tests(DecodeBoundaryTestRunner& runner) {
    runner.run("media_probe_requires_complete_truth", media_probe_requires_complete_truth);
    runner.run("open_result_rejects_invalid_probe", open_result_rejects_invalid_probe);
    runner.run(
        "decoded_block_requires_exact_frame_bytes",
        decoded_block_requires_exact_frame_bytes);
    runner.run(
        "step_result_keeps_eos_distinct_from_failure",
        step_result_keeps_eos_distinct_from_failure);
    runner.run(
        "unsupported_seek_and_flush_are_explicit",
        unsupported_seek_and_flush_are_explicit);
}
