#pragma once

#include <cstdint>

namespace kivo::core::processing {

enum class ResampleQuality : uint8_t {
    Fast = 0,
    Balanced,
    High
};

} // namespace kivo::core::processing
