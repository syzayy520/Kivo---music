#pragma once

#include <cstdint>

namespace kivo::core::render {

enum class RenderLifecycleState : uint8_t {
    Closed = 0,
    Open,
    Started,
    Draining,
    Stopped,
    Failed
};

} // namespace kivo::core::render
