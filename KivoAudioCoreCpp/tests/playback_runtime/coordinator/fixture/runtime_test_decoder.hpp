#pragma once

#include <array>
#include <cstddef>
#include <memory>

#include "kivo/core/decode/boundary/audio_decode_boundary.hpp"

#include "runtime_test_call_gate.hpp"

namespace playback_runtime_test {

class ScriptedDecoder final
    : public kivo::core::decode::AudioDecodeBoundary {
public:
    bool fail_open{false};
    bool fail_seek{false};
    bool emit_stale_after_seek{false};
    kivo::core::decode::DecodeFailure terminal_failure{
        kivo::core::decode::DecodeFailure::None};
    RuntimeTestCallGate* open_gate{nullptr};
    uint64_t close_count{0};
    uint64_t seek_count{0};
    kivo::core::contract::SamplePosition last_seek_target{0};
    kivo::core::processing::ResampleQuality last_resample_quality{
        kivo::core::processing::ResampleQuality::Balanced};
    kivo::core::processing::DitherPolicy last_conversion_dither{
        kivo::core::processing::DitherPolicy::Disabled};

    [[nodiscard]] kivo::core::decode::DecodeOpenResult open(
        std::unique_ptr<kivo::core::decode::ByteSourceBoundary> source,
        const kivo::core::decode::DecodeOpenRequest& request) noexcept override {
        if (open_gate) {
            open_gate->enter_and_wait();
        }
        if (!source || fail_open) {
            opened_ = false;
            return kivo::core::decode::DecodeOpenResult::failed(
                kivo::core::decode::DecodeFailure::SourceOpenFailed);
        }
        opened_ = true;
        source_identity_ = source->identity();
        source_generation_ = source->generation();
        decode_generation_ = request.decode_generation;
        previous_decode_generation_ = request.decode_generation;
        output_format_ = request.target_format;
        last_resample_quality = request.resample_quality;
        last_conversion_dither = request.conversion_dither;
        next_block_ = 0;
        seeked_ = false;
        probe_ = make_probe();
        return kivo::core::decode::DecodeOpenResult::accepted(probe_);
    }

    [[nodiscard]] kivo::core::decode::DecodeStepResult
    decode_next() noexcept override {
        if (!opened_) {
            return kivo::core::decode::DecodeStepResult::failed(
                kivo::core::decode::DecodeFailure::BoundaryFailure);
        }
        if (seeked_ && emit_stale_after_seek && next_block_ == 0) {
            ++next_block_;
            return kivo::core::decode::DecodeStepResult::produced(
                make_block(previous_decode_generation_));
        }
        const size_t current_index = next_block_++;
        const size_t produced_index =
            seeked_ && emit_stale_after_seek
            ? current_index - 1
            : current_index;
        if (produced_index == 0) {
            return kivo::core::decode::DecodeStepResult::produced(
                make_block(decode_generation_));
        }
        return terminal_failure
                == kivo::core::decode::DecodeFailure::None
            ? kivo::core::decode::DecodeStepResult::end_of_stream()
            : kivo::core::decode::DecodeStepResult::failed(
                terminal_failure);
    }

    [[nodiscard]] kivo::core::decode::DecodeControlResult seek(
        kivo::core::contract::SamplePosition target_frame,
        kivo::core::decode::DecodeGeneration generation) noexcept override {
        if (!opened_ || fail_seek) {
            return kivo::core::decode::DecodeControlResult::failed(
                kivo::core::decode::DecodeFailure::SourceSeekFailed);
        }
        previous_decode_generation_ = decode_generation_;
        decode_generation_ = generation;
        target_frame_ = target_frame;
        last_seek_target = target_frame;
        ++seek_count;
        next_block_ = 0;
        seeked_ = true;
        probe_.decode_generation = generation;
        return kivo::core::decode::DecodeControlResult::succeeded();
    }

    [[nodiscard]] kivo::core::decode::DecodeControlResult flush(
        kivo::core::decode::DecodeGeneration generation) noexcept override {
        if (!opened_) {
            return kivo::core::decode::DecodeControlResult::failed(
                kivo::core::decode::DecodeFailure::BoundaryFailure);
        }
        previous_decode_generation_ = decode_generation_;
        decode_generation_ = generation;
        next_block_ = 0;
        probe_.decode_generation = generation;
        return kivo::core::decode::DecodeControlResult::succeeded();
    }

    [[nodiscard]] kivo::core::decode::DecodeControlResult close()
        noexcept override {
        opened_ = false;
        ++close_count;
        return kivo::core::decode::DecodeControlResult::succeeded();
    }

    [[nodiscard]] kivo::core::decode::MediaProbe probe()
        const noexcept override {
        return probe_;
    }

private:
    [[nodiscard]] kivo::core::decode::MediaProbe make_probe()
        const noexcept {
        kivo::core::decode::MediaProbe probe{};
        probe.source_identity = source_identity_;
        probe.source_generation = source_generation_;
        probe.decode_generation = decode_generation_;
        probe.codec = kivo::core::decode::AudioCodec::Pcm;
        probe.container = kivo::core::decode::MediaContainer::Wave;
        probe.native_format = {output_format_.format};
        probe.output_format = output_format_;
        probe.resample = {
            false,
            kivo::core::contract::ResampleReason::NotNeeded,
            output_format_.format.sample_rate,
            output_format_.format.sample_rate
        };
        probe.duration_frames = 1000;
        probe.duration_known = true;
        probe.seekable = true;
        return probe;
    }

    [[nodiscard]] kivo::core::decode::DecodedAudioBlockView make_block(
        kivo::core::decode::DecodeGeneration generation) const noexcept {
        return {
            bytes_,
            output_format_,
            4,
            target_frame_,
            source_generation_,
            generation,
            false
        };
    }

    std::array<std::byte, 32> bytes_{};
    bool opened_{false};
    bool seeked_{false};
    size_t next_block_{0};
    kivo::core::contract::SamplePosition target_frame_{0};
    kivo::core::contract::SourceIdentity source_identity_{};
    kivo::core::contract::SourceGeneration source_generation_{};
    kivo::core::decode::DecodeGeneration decode_generation_{};
    kivo::core::decode::DecodeGeneration previous_decode_generation_{};
    kivo::core::contract::RenderFormat output_format_{};
    kivo::core::decode::MediaProbe probe_{};
};

} // namespace playback_runtime_test
