#pragma once

#include <string>

#include "../../../fixture/device_matrix_record.hpp"

struct IMMDevice;

namespace device_matrix::windows {

[[nodiscard]] bool probe_endpoint_metadata(
    IMMDevice& device,
    const std::wstring& endpoint_id,
    bool default_console,
    bool default_multimedia,
    bool default_communications,
    EndpointRecord& record,
    long& platform_code);

} // namespace device_matrix::windows
