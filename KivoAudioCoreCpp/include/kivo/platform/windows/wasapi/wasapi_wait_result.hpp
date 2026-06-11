#pragma once

#include <cstdint>

namespace kivo::platform::windows::wasapi {

enum class WasapiWaitResult : uint8_t {
    Ready = 0,
    Timeout,
    Closed,
    DeviceLost,
    Failed
};

} // namespace kivo::platform::windows::wasapi
