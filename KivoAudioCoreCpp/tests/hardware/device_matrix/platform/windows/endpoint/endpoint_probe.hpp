#pragma once

#include "../../../fixture/device_matrix_record.hpp"

struct IMMDevice;

namespace device_matrix::windows {

[[nodiscard]] bool probe_endpoint(
    IMMDevice& device,
    bool default_console,
    bool default_multimedia,
    bool default_communications,
    EndpointRecord& record,
    long& platform_code);

} // namespace device_matrix::windows
