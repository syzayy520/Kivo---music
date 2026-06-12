#pragma once

#include <sstream>

#include "kivo/adapters/ffmpeg/session/ffmpeg_audio_decode_session.hpp"
#include "kivo/source/local/local_file_byte_source.hpp"

namespace ffmpeg_decode_test {

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

struct FixtureExpectation {
    const char* file_name;
    kivo::core::decode::AudioCodec codec;
    kivo::core::decode::MediaContainer container;
};

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
    uint64_t identity = 101) {
    return kivo::source::local::LocalFileByteSource::open(
        path,
        {identity},
        {generation(7)});
}

[[nodiscard]] constexpr kivo::core::decode::DecodeOpenRequest open_request(
    bool allow_conversion = true) noexcept {
    return {
        target_format(),
        {generation(11)},
        allow_conversion
    };
}

inline void require_opened(
    const kivo::core::decode::DecodeOpenResult& result) {
    if (!result.is_accepted()) {
        std::ostringstream message;
        message << "open failed: disposition="
                << static_cast<int>(result.disposition())
                << " failure=" << static_cast<int>(result.failure());
        throw std::runtime_error(message.str());
    }
}

} // namespace ffmpeg_decode_test
