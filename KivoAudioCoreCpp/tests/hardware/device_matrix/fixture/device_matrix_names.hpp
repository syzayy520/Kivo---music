#pragma once

#include "device_matrix_record.hpp"

namespace device_matrix {

[[nodiscard]] constexpr const char* category_name(
    DeviceCategory category) noexcept {
    switch (category) {
    case DeviceCategory::Integrated: return "integrated";
    case DeviceCategory::Usb: return "usb";
    case DeviceCategory::Bluetooth: return "bluetooth";
    case DeviceCategory::DisplayAudio: return "display-audio";
    case DeviceCategory::Other: return "other";
    case DeviceCategory::Unknown: return "unknown";
    }
    return "unknown";
}

[[nodiscard]] constexpr const char* state_name(
    EndpointState state) noexcept {
    switch (state) {
    case EndpointState::Active: return "active";
    case EndpointState::Disabled: return "disabled";
    case EndpointState::NotPresent: return "not-present";
    case EndpointState::Unplugged: return "unplugged";
    case EndpointState::Unknown: break;
    }
    return "unknown";
}

} // namespace device_matrix
