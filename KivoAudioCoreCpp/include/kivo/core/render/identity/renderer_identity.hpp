#pragma once

#include <cstdint>

namespace kivo::core::render {

struct RendererIdentity {
    uint64_t value{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return value != 0;
    }

    [[nodiscard]] constexpr bool operator==(const RendererIdentity&) const noexcept = default;
};

} // namespace kivo::core::render
