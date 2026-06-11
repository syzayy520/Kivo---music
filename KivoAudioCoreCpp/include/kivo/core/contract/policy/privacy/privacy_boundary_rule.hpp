#pragma once
#include <cstdint>

namespace kivo::core::contract::policy {

struct PrivacyBoundaryRule {
    uint8_t expose_local_paths{0};
    uint8_t expose_user_names{0};
    uint8_t expose_file_metadata{0};

    [[nodiscard]] constexpr bool operator==(const PrivacyBoundaryRule&) const noexcept = default;
};

} // namespace kivo::core::contract::policy
