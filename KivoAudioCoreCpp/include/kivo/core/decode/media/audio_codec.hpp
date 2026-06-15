#pragma once

#include <cstdint>

namespace kivo::core::decode {

enum class AudioCodec : uint8_t {
    Unknown = 0,
    Pcm,
    Flac,
    Mp3,
    Aac,
    Vorbis,
    Opus,
    Dsd,
    Alac
};

} // namespace kivo::core::decode
