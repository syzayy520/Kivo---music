#include "device_matrix_validation.hpp"

#include <algorithm>

namespace device_matrix {

bool validate_inventory(const DeviceInventory& inventory) noexcept {
    if (inventory.platform_code != 0 || inventory.endpoints.empty()) {
        return false;
    }
    const auto valid_endpoint = [](const EndpointRecord& endpoint) {
        if (endpoint.identity == 0
            || endpoint.friendly_name.empty()
            || endpoint.state == 0) {
            return false;
        }
        if (!endpoint.is_active()) {
            return !endpoint.default_console
                && !endpoint.default_multimedia
                && !endpoint.default_communications
                && !endpoint.mix_format.is_valid();
        }
        return endpoint.mix_format.is_valid()
            && endpoint.shared_mix_supported
            && endpoint.default_period_100ns > 0
            && endpoint.minimum_period_100ns > 0;
    };
    if (!std::all_of(
            inventory.endpoints.begin(),
            inventory.endpoints.end(),
            valid_endpoint)) {
        return false;
    }
    return std::any_of(
        inventory.endpoints.begin(),
        inventory.endpoints.end(),
        [](const EndpointRecord& endpoint) {
            return endpoint.is_active()
                && (endpoint.default_console
                    || endpoint.default_multimedia
                    || endpoint.default_communications);
        });
}

} // namespace device_matrix
