#include "render_boundary_tests_main.hpp"

#include <array>
#include <cstddef>

#include "kivo/core/render/buffer/audio_block_view.hpp"

namespace {

using namespace kivo::core;

contract::RenderFormat stereo_float_format() {
    return {{
        contract::SampleFormat::Float32,
        contract::ChannelLayout::Stereo,
        contract::FrameLayout::Interleaved,
        {},
        48000,
        32,
        32
    }};
}

void audio_block_requires_exact_byte_shape() {
    std::array<std::byte, 32> bytes{};
    render::AudioBlockView block{
        bytes,
        stereo_float_format(),
        4,
        100,
        {9},
        {},
        {},
        false
    };
    RENDER_ASSERT(block.is_valid());

    block.frame_count = 5;
    RENDER_ASSERT(!block.is_valid());
}

void audio_block_rejects_invalid_frame_offset() {
    std::array<std::byte, 8> bytes{};
    render::AudioBlockView block{
        bytes,
        stereo_float_format(),
        1,
        contract::kInvalidSamplePosition,
        {1},
        {},
        {},
        false
    };
    RENDER_ASSERT(!block.is_valid());
}

void partial_write_tail_advances_without_allocation() {
    std::array<std::byte, 32> bytes{};
    const render::AudioBlockView block{
        bytes,
        stereo_float_format(),
        4,
        100,
        {3},
        {},
        {},
        true
    };

    const auto tail = block.tail_after(1);
    RENDER_ASSERT(tail.has_value());
    RENDER_ASSERT(tail->is_valid());
    RENDER_ASSERT(tail->frame_count == 3);
    RENDER_ASSERT(tail->frame_offset == 101);
    RENDER_ASSERT(tail->bytes.size() == 24);
    RENDER_ASSERT(tail->bytes.data() == bytes.data() + 8);
    RENDER_ASSERT(tail->end_of_stream);
}

void full_consumption_has_no_retry_view() {
    std::array<std::byte, 8> bytes{};
    const render::AudioBlockView block{
        bytes,
        stereo_float_format(),
        1,
        0,
        {5},
        {},
        {},
        false
    };
    RENDER_ASSERT(!block.tail_after(1).has_value());
}

} // namespace

void run_audio_block_view_tests(RenderBoundaryTestRunner& runner) {
    runner.run("audio_block_requires_exact_byte_shape", audio_block_requires_exact_byte_shape);
    runner.run("audio_block_rejects_invalid_frame_offset", audio_block_rejects_invalid_frame_offset);
    runner.run(
        "partial_write_tail_advances_without_allocation",
        partial_write_tail_advances_without_allocation);
    runner.run("full_consumption_has_no_retry_view", full_consumption_has_no_retry_view);
}
