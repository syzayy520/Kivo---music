#pragma once

#include <cstddef>
#include <memory>

#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/decode/generation/decode_generation.hpp"
#include "kivo/core/contract/source/identity/source_generation.hpp"

namespace kivo::core::playback::pipeline_buffer {

struct BufferedBlock {
    std::unique_ptr<std::byte[]> bytes{};
    size_t byte_count{0};
    contract::FrameCount frame_count{0};
    contract::SamplePosition frame_offset{0};
    contract::SourceGeneration source_generation{};
    decode::DecodeGeneration decode_generation{};
};

enum class StoreResult : uint8_t {
    Stored = 0,
    DiscardedStale,
    Failed
};

} // namespace kivo::core::playback::pipeline_buffer
