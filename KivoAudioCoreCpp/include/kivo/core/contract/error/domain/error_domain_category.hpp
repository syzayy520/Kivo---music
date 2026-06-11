#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class ErrorDomainCategory : uint8_t {
    Unknown = 0,
    User,
    System,
    Resource,
    Codec,
    Source
};

} // namespace kivo::core::contract