#pragma once

#include <array>
#include <memory>

#include "kivo/core/decode/boundary/audio_decode_boundary.hpp"

namespace playback_pipeline_test {

class ScriptedDecoder final
    : public kivo::core::decode::AudioDecodeBoundary {
public:
    std::array<kivo::core::decode::DecodedAudioBlockView, 2> blocks{};
    size_t block_count{0};
    size_t next_block{0};
    kivo::core::decode::DecodeFailure terminal_failure{
        kivo::core::decode::DecodeFailure::None};

    [[nodiscard]] kivo::core::decode::DecodeOpenResult open(
        std::unique_ptr<kivo::core::decode::ByteSourceBoundary>,
        const kivo::core::decode::DecodeOpenRequest&) noexcept override {
        return kivo::core::decode::DecodeOpenResult::failed(
            kivo::core::decode::DecodeFailure::BoundaryFailure);
    }

    [[nodiscard]] kivo::core::decode::DecodeStepResult
    decode_next() noexcept override {
        if (next_block < block_count) {
            return kivo::core::decode::DecodeStepResult::produced(
                blocks[next_block++]);
        }
        return terminal_failure
                == kivo::core::decode::DecodeFailure::None
            ? kivo::core::decode::DecodeStepResult::end_of_stream()
            : kivo::core::decode::DecodeStepResult::failed(
                terminal_failure);
    }

    [[nodiscard]] kivo::core::decode::DecodeControlResult seek(
        kivo::core::contract::SamplePosition,
        kivo::core::decode::DecodeGeneration) noexcept override {
        return kivo::core::decode::DecodeControlResult::no_op();
    }

    [[nodiscard]] kivo::core::decode::DecodeControlResult flush(
        kivo::core::decode::DecodeGeneration) noexcept override {
        return kivo::core::decode::DecodeControlResult::no_op();
    }

    [[nodiscard]] kivo::core::decode::DecodeControlResult close()
        noexcept override {
        return kivo::core::decode::DecodeControlResult::succeeded();
    }

    [[nodiscard]] kivo::core::decode::MediaProbe probe()
        const noexcept override {
        return {};
    }
};

} // namespace playback_pipeline_test
