#pragma once

#include "../../../fixture/device_matrix_record.hpp"

namespace device_matrix::windows {

[[nodiscard]] DeviceInventory enumerate_active_render_endpoints();

} // namespace device_matrix::windows
