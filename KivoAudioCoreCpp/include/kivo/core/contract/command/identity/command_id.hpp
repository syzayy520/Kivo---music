#pragma once

#include <cstdint>

namespace kivo::core::contract {

struct CommandId {
    uint64_t value{0};
    [[nodiscard]] bool operator==(const CommandId&) const noexcept = default;
};

} // namespace kivo::core::contract