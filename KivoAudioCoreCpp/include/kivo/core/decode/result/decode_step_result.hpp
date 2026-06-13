#pragma once

#include <cstdint>

#include "kivo/core/decode/block/decoded_audio_block_view.hpp"
#include "kivo/core/decode/failure/decode_failure.hpp"

namespace kivo::core::decode {

enum class DecodeStepDisposition : uint8_t {
    Produced = 0,
    EndOfStream,
    Failed
};

class [[nodiscard]] DecodeStepResult {
public:
    [[nodiscard]] static constexpr DecodeStepResult produced(
        DecodedAudioBlockView block) noexcept {
        return block.is_valid()
            ? DecodeStepResult{
                DecodeStepDisposition::Produced,
                DecodeFailure::None,
                block}
            : failed(DecodeFailure::InvalidBuffer);
    }

    [[nodiscard]] static constexpr DecodeStepResult end_of_stream() noexcept {
        return {
            DecodeStepDisposition::EndOfStream,
            DecodeFailure::EndOfStream,
            {}
        };
    }

    [[nodiscard]] static constexpr DecodeStepResult failed(
        DecodeFailure failure) noexcept {
        return {
            DecodeStepDisposition::Failed,
            failure == DecodeFailure::None
                ? DecodeFailure::BoundaryFailure
                : failure,
            {}
        };
    }

    [[nodiscard]] constexpr DecodeStepDisposition disposition() const noexcept {
        return disposition_;
    }

    [[nodiscard]] constexpr DecodeFailure failure() const noexcept {
        return failure_;
    }

    [[nodiscard]] constexpr const DecodedAudioBlockView& block() const noexcept {
        return block_;
    }

    [[nodiscard]] constexpr bool operator==(
        const DecodeStepResult&) const noexcept = default;

private:
    constexpr DecodeStepResult(
        DecodeStepDisposition disposition,
        DecodeFailure failure,
        DecodedAudioBlockView block) noexcept
        : disposition_(disposition), failure_(failure), block_(block) {}

    DecodeStepDisposition disposition_;
    DecodeFailure failure_;
    DecodedAudioBlockView block_;
};

} // namespace kivo::core::decode
