#pragma once

#include <cstdint>

namespace kivo::core::playback {

enum class QueueEndOfStreamPolicy : uint8_t {
    CloseAndMarkFinal = 0,
    KeepOpen
};

} // namespace kivo::core::playback
