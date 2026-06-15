#pragma once

#include "../fixture/device_matrix_record.hpp"

namespace device_matrix {

[[nodiscard]] bool validate_inventory(
    const DeviceInventory& inventory) noexcept;

} // namespace device_matrix
