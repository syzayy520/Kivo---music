#include "device_matrix_report.hpp"

#include <iomanip>
#include <ostream>

namespace device_matrix {

namespace {

[[nodiscard]] const char* yes_no(bool value) noexcept {
    return value ? "yes" : "no";
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
    output << "# WASAPI Active Render Endpoint Inventory\n\n";
    output << "Platform code: `" << inventory.platform_code << "`\n\n";
    output
        << "| Identity | Name | Driver | Form factor | Default roles | "
           "Mix format | Shared | Exclusive | Period default/min (100 ns) |\n"
        << "|---|---|---|---|---|---|---|---|---|\n";

    for (const auto& endpoint : inventory.endpoints) {
        output << "| `0x"
               << std::hex << std::uppercase << endpoint.identity
               << std::dec << std::nouppercase
               << "` | " << endpoint.friendly_name
               << " | "
               << (endpoint.driver_version.empty()
                    ? "unavailable"
                    : endpoint.driver_version)
               << " | " << endpoint.form_factor
               << " | ";
        write_roles(output, endpoint);
        output << " | "
               << endpoint.mix_format.sample_rate << " Hz, "
               << endpoint.mix_format.channels << " ch, "
               << endpoint.mix_format.bits_per_sample << " bit"
               << " | " << yes_no(endpoint.shared_mix_supported)
               << " | " << yes_no(endpoint.exclusive_mix_supported)
               << " | " << endpoint.default_period_100ns
               << " / " << endpoint.minimum_period_100ns
               << " |\n";
    }
}

} // namespace device_matrix
