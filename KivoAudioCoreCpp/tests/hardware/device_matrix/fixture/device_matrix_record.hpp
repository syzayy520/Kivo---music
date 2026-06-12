#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace device_matrix {

enum class DeviceCategory : uint8_t {
    Unknown = 0,
    Integrated,
    Usb,
    Bluetooth,
    DisplayAudio,
    Other
};

struct EndpointFormat final {
    uint16_t format_tag{0};
    uint16_t channels{0};
    uint32_t sample_rate{0};
    uint16_t bits_per_sample{0};
    uint16_t block_alignment{0};

    [[nodiscard]] bool is_valid() const noexcept {
        return channels != 0
            && sample_rate != 0
            && bits_per_sample != 0
            && block_alignment != 0;
    }
};

struct EndpointRecord final {
    uint64_t identity{0};
    std::string friendly_name;
    std::string endpoint_driver_version;
    std::string parent_device_name;
    std::string parent_driver_provider;
    std::string parent_driver_version;
    std::string form_factor;
    DeviceCategory category{DeviceCategory::Unknown};
    uint32_t state{0};
    bool default_console{false};
    bool default_multimedia{false};
    bool default_communications{false};
    bool shared_mix_supported{false};
    bool exclusive_mix_supported{false};
    int64_t default_period_100ns{0};
    int64_t minimum_period_100ns{0};
    EndpointFormat mix_format{};

    [[nodiscard]] bool is_active() const noexcept {
        return (state & 0x00000001u) != 0;
    }
};

struct DeviceInventory final {
    std::vector<EndpointRecord> endpoints;
    long platform_code{0};
};

} // namespace device_matrix
