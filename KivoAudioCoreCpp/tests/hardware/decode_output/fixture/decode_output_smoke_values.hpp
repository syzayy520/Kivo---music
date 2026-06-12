#pragma once

#include <cstdint>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/generation_id.hpp"

namespace kivo::hardware_test::decode_output {

[[nodiscard]] constexpr core::contract::GenerationId generation(
    uint64_t value) noexcept {
    auto result = core::contract::GenerationId::initial();
    for (uint64_t index = 0; index < value; ++index) {
        result = core::contract::GenerationId::next(result);
    }
    return result;
}

[[nodiscard]] inline core::contract::RenderFormat preferred_format() noexcept {
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

} // namespace kivo::hardware_test::decode_output
