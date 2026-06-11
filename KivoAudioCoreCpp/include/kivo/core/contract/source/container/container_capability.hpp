#pragma once

#include <cstdint>

namespace kivo::core::contract {

/// @brief Container capability — what features the container supports.
struct ContainerCapability {
    bool supports_chapters{false};
    bool supports_attachments{false};
    bool supports_multiple_streams{false};

    [[nodiscard]] constexpr bool operator==(const ContainerCapability&) const noexcept = default;
};

} // namespace kivo::core::contract
