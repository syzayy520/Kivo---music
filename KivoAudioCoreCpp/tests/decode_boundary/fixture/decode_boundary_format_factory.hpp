#pragma once

#include "kivo/core/contract/format/roles/native_decoded_format.hpp"
#include "kivo/core/contract/format/roles/render_format.hpp"

namespace kivo::decode_boundary::test {

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

} // namespace kivo::decode_boundary::test
