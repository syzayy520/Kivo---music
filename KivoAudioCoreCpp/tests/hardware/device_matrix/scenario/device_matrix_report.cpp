#include "device_matrix_report.hpp"

#include <iomanip>
#include <ostream>

namespace device_matrix {

namespace {

[[nodiscard]] const char* yes_no(bool value) noexcept {
    return value ? "yes" : "no";
}

[[nodiscard]] const char* category_name(DeviceCategory category) noexcept {
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

[[nodiscard]] const char* state_name(EndpointState state) noexcept {
    switch (state) {
    case EndpointState::Active: return "active";
    case EndpointState::Disabled: return "disabled";
    case EndpointState::NotPresent: return "not-present";
    case EndpointState::Unplugged: return "unplugged";
    case EndpointState::Unknown: break;
    }
    return "unknown";
}

void write_roles(std::ostream& output, const EndpointRecord& endpoint) {
    bool wrote_role = false;
    const auto write_role = [&](bool enabled, const char* name) {
        if (!enabled) {
            return;
        }
        if (wrote_role) {
            output << ',';
        }
        output << name;
        wrote_role = true;
    };
    write_role(endpoint.default_console, "console");
    write_role(endpoint.default_multimedia, "multimedia");
    write_role(endpoint.default_communications, "communications");
    if (!wrote_role) {
        output << "none";
    }
}

} // namespace

void write_device_matrix_report(
    std::ostream& output,
    const DeviceInventory& inventory) {
    output << "# WASAPI Render Endpoint Inventory\n\n";
    output << "Platform code: `" << inventory.platform_code << "`\n\n";
    output
        << "| Identity | Name | Category | State | Parent driver | "
           "Form factor | Default roles | "
           "Mix format | Shared | Exclusive | Period default/min (100 ns) |\n"
        << "|---|---|---|---|---|---|---|---|---|---|---|\n";

    for (const auto& endpoint : inventory.endpoints) {
        output << "| `0x"
               << std::hex << std::uppercase << endpoint.identity
               << std::dec << std::nouppercase
               << "` | " << endpoint.friendly_name
               << " | " << category_name(endpoint.category)
               << " | " << state_name(endpoint.state)
               << " | " << (endpoint.parent_device_name.empty()
                    ? "unavailable"
                    : endpoint.parent_device_name)
               << " / " << (endpoint.parent_driver_provider.empty()
                    ? "unknown"
                    : endpoint.parent_driver_provider)
               << " / " << (endpoint.parent_driver_version.empty()
                    ? "unknown"
                    : endpoint.parent_driver_version)
               << " | " << endpoint.form_factor
               << " | ";
        write_roles(output, endpoint);
        if (endpoint.is_active()) {
            output << " | "
                   << endpoint.mix_format.sample_rate << " Hz, "
                   << endpoint.mix_format.channels << " ch, "
                   << endpoint.mix_format.bits_per_sample << " bit"
                   << " | " << yes_no(endpoint.shared_mix_supported)
                   << " | " << yes_no(endpoint.exclusive_mix_supported)
                   << " | " << endpoint.default_period_100ns
                   << " / " << endpoint.minimum_period_100ns;
        } else {
            output << " | unavailable | unavailable | unavailable | unavailable";
        }
        output << " |\n";
    }
}

} // namespace device_matrix
