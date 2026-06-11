#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class DeviceLostDuringFlushRule : uint8_t {
    Unknown = 0,
    CancelFlush,
    RetryFlush,
    ForceFlush,
    RejectFlush,
    DeferFlush
};

} // namespace kivo::core::contract