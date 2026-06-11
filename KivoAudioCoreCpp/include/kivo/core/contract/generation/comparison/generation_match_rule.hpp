#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class GenerationMatchRule : uint8_t {
    NotSpecified = 0,
    MustMatch,
    MayDiffer,
    MustBeNewer
};

} // namespace kivo::core::contract