#pragma once

#include <cstdint>

namespace kivo::core::processing {

enum class DitherPolicy : uint8_t {
    Disabled = 0,
    TriangularPdf
};

} // namespace kivo::core::processing
