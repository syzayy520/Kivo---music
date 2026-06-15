#pragma once

#include "kivo/adapters/ffmpeg/session/ffmpeg_audio_decode_session.hpp"
#include "kivo/source/local/local_file_byte_source.hpp"

namespace stability_test {

[[nodiscard]] constexpr kivo::core::contract::RenderFormat
target_format() noexcept {
    using namespace kivo::core::contract;
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

[[nodiscard]] constexpr kivo::core::contract::GenerationId generation(
    uint64_t value) noexcept {
    auto result = kivo::core::contract::GenerationId::initial();
    for (uint64_t index = 0; index < value; ++index) {
        result = kivo::core::contract::GenerationId::next(result);
    }
    return result;
}

[[nodiscard]] inline std::unique_ptr<kivo::core::decode::ByteSourceBoundary>
open_source(
    const std::filesystem::path& path,
    uint64_t identity) {
    return kivo::source::local::LocalFileByteSource::open(
        path,
        {identity},
        {generation(identity)});
}

[[nodiscard]] constexpr kivo::core::decode::DecodeOpenRequest
open_request(uint64_t generation_value) noexcept {
    return {
        target_format(),
        {generation(generation_value)},
        true
    };
}

} // namespace stability_test
