#pragma once

#include <cstdint>

namespace kivo::core::render {

enum class RenderOutputMode : uint8_t {
    Shared = 0,
    Exclusive
};

} // namespace kivo::core::render
