#pragma once

#include "../../../fixture/device_matrix_record.hpp"

struct IMMDevice;

namespace device_matrix::windows {

[[nodiscard]] bool probe_endpoint_capabilities(
    IMMDevice& device,
    EndpointRecord& record,
    long& platform_code);

} // namespace device_matrix::windows
