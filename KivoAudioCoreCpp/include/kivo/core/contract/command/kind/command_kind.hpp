#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class CommandKind : uint8_t {
    Unknown = 0,
    OpenSource,
    CloseSource,
    Seek,
    Pause,
    Resume,
    Stop,
    SkipForward,
    SkipBackward,
    Flush,
    SetVolume,
    SetOutput
};

} // namespace kivo::core::contract