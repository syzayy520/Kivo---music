#pragma once

#include <string>

#include "../../../fixture/device_matrix_record.hpp"

namespace device_matrix::windows {

struct PnpDeviceRecord final {
    std::string endpoint_driver_version;
    std::string parent_device_name;
    std::string parent_driver_provider;
    std::string parent_driver_version;
    std::wstring parent_instance_id;
};

[[nodiscard]] PnpDeviceRecord probe_pnp_device(
    const std::wstring& endpoint_id);
[[nodiscard]] DeviceCategory classify_device(
    const PnpDeviceRecord& device,
    const std::string& form_factor);

} // namespace device_matrix::windows
