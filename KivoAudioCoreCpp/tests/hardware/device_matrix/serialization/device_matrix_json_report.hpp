#pragma once

#include <iosfwd>
#include <string_view>

#include "../fixture/device_matrix_record.hpp"

namespace device_matrix {

void write_device_matrix_json(
    std::ostream& output,
    const DeviceInventory& inventory,
    std::string_view machine_id,
    bool inventory_valid);

} // namespace device_matrix
