#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class ErrorSeverity : uint8_t {
    Unknown = 0,
    Fatal,
    Recoverable,
    Warning,
    Info
};

} // namespace kivo::core::contract