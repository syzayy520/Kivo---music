#pragma once

#include <cstdint>

namespace kivo::core::decode {

enum class MediaContainer : uint8_t {
    Unknown = 0,
    Wave,
    Flac,
    Mp3,
    Mp4,
    Ogg,
    Matroska
};

} // namespace kivo::core::decode
