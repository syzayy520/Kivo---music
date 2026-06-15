#pragma once

#include <cstddef>
#include <limits>
#include <optional>
#include <span>

#include "kivo/core/contract/buffer/identity/buffer_generation.hpp"
#include "kivo/core/contract/buffer/identity/buffer_id.hpp"
#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/render/generation/render_generation_set.hpp"

namespace kivo::core::render {

struct AudioBlockView {
    std::span<const std::byte> bytes{};
    contract::RenderFormat format{};
    contract::FrameCount frame_count{0};
    contract::SamplePosition frame_offset{contract::kInvalidSamplePosition};
    contract::BufferId buffer_id{};
    contract::BufferGeneration buffer_generation{};
    RenderGenerationSet generations{};
    bool end_of_stream{false};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        if (!format.is_valid() || frame_count == 0) {
            return false;
        }
        if (frame_offset == contract::kInvalidSamplePosition) {
            return false;
        }

        const auto bytes_per_frame = format.format.bytes_per_frame();
        if (bytes_per_frame == 0) {
            return false;
        }
        if (frame_count > std::numeric_limits<size_t>::max() / bytes_per_frame) {
            return false;
        }
        if (frame_offset > std::numeric_limits<contract::SamplePosition>::max() - frame_count) {
            return false;
        }

        const auto expected_bytes = static_cast<size_t>(frame_count) * bytes_per_frame;
        return bytes.size() == expected_bytes;
    }

    [[nodiscard]] constexpr std::optional<AudioBlockView> tail_after(
        contract::FrameCount accepted_frames) const noexcept {
        if (!is_valid() || accepted_frames >= frame_count) {
            return std::nullopt;
        }

        const auto bytes_per_frame = format.format.bytes_per_frame();
        const auto accepted_bytes = static_cast<size_t>(accepted_frames) * bytes_per_frame;
        auto tail = *this;
        tail.bytes = bytes.subspan(accepted_bytes);
        tail.frame_count -= accepted_frames;
        tail.frame_offset += accepted_frames;
        return tail;
    }

    [[nodiscard]] constexpr bool operator==(const AudioBlockView&) const noexcept = default;
};

} // namespace kivo::core::render
