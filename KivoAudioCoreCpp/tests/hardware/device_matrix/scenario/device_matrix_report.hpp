#pragma once

#include <iosfwd>

#include "../fixture/device_matrix_record.hpp"

namespace device_matrix {

void write_device_matrix_report(
    std::ostream& output,
    const DeviceInventory& inventory);

} // namespace device_matrix
