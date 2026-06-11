#pragma once

#include <cstddef>
#include <limits>
#include <span>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/contract/source/identity/source_generation.hpp"
#include "kivo/core/decode/generation/decode_generation.hpp"

namespace kivo::core::decode {

struct DecodedAudioBlockView {
    std::span<const std::byte> bytes{};
    contract::RenderFormat format{};
    contract::FrameCount frame_count{0};
    contract::SamplePosition frame_offset{contract::kInvalidSamplePosition};
    contract::SourceGeneration source_generation{};
    DecodeGeneration decode_generation{};
    bool end_of_stream{false};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        if (!format.is_valid()
            || frame_count == 0
            || frame_offset == contract::kInvalidSamplePosition) {
            return false;
        }
        const auto bytes_per_frame = format.format.bytes_per_frame();
        if (bytes_per_frame == 0
            || frame_count > std::numeric_limits<size_t>::max() / bytes_per_frame
            || frame_offset
                > std::numeric_limits<contract::SamplePosition>::max() - frame_count) {
            return false;
        }
        return bytes.size() == static_cast<size_t>(frame_count) * bytes_per_frame;
    }

    [[nodiscard]] constexpr bool operator==(
        const DecodedAudioBlockView&) const noexcept = default;
};

} // namespace kivo::core::decode
