#pragma once

#include <cstdint>

#include "../kind/command_origin.hpp"

namespace kivo::core::contract {

struct CommandGenerationRef {
    uint64_t generation{0};
    CommandOrigin origin{CommandOrigin::Unknown};
    [[nodiscard]] bool operator==(const CommandGenerationRef&) const noexcept = default;
};

} // namespace kivo::core::contract