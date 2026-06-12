#include "endpoint_metadata_probe.hpp"

#include <Windows.h>
#include <mmdeviceapi.h>
#include <propsys.h>
#include <propvarutil.h>
#include <wrl/client.h>

#include "../driver/pnp_device_probe.hpp"

namespace device_matrix::windows {

namespace {

using Microsoft::WRL::ComPtr;

constexpr PROPERTYKEY kDeviceFriendlyName{
    {0xa45c254e, 0xdf1c, 0x4efd,
     {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14
};
constexpr PROPERTYKEY kAudioEndpointFormFactor{
    {0x1da5d803, 0xd492, 0x4edd,
     {0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e}},
    0
};

[[nodiscard]] uint64_t stable_endpoint_hash(
    const std::wstring& value) noexcept {
    constexpr uint64_t offset_basis = 14695981039346656037ull;
    constexpr uint64_t prime = 1099511628211ull;
    uint64_t hash = offset_basis;
    for (const auto character : value) {
        hash ^= static_cast<uint16_t>(character);
        hash *= prime;
    }
    return hash == 0 ? 1 : hash;
}

[[nodiscard]] std::string utf8(const wchar_t* value) {
    if (value == nullptr || *value == L'\0') {
        return {};
    }
    const auto required = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        value,
        -1,
        nullptr,
        0,
        nullptr,
        nullptr);
    if (required <= 1) {
        return {};
    }
    std::string result(static_cast<size_t>(required), '\0');
    if (WideCharToMultiByte(
            CP_UTF8,
            WC_ERR_INVALID_CHARS,
            value,
            -1,
            result.data(),
            required,
            nullptr,
            nullptr) != required) {
        return {};
    }
    result.resize(static_cast<size_t>(required - 1));
    return result;
}

[[nodiscard]] std::string read_string(
    IPropertyStore& properties,
    REFPROPERTYKEY key) {
    PROPVARIANT value;
    PropVariantInit(&value);
    const auto code = properties.GetValue(key, &value);
    std::string result;
    if (SUCCEEDED(code) && value.vt == VT_LPWSTR) {
        result = utf8(value.pwszVal);
    }
    PropVariantClear(&value);
    return result;
}

[[nodiscard]] const char* form_factor_name(UINT value) noexcept {
    switch (static_cast<EndpointFormFactor>(value)) {
    case RemoteNetworkDevice: return "remote-network";
    case Speakers: return "speakers";
    case LineLevel: return "line-level";
    case Headphones: return "headphones";
    case Microphone: return "microphone";
    case Headset: return "headset";
    case Handset: return "handset";
    case UnknownDigitalPassthrough: return "digital-passthrough";
    case SPDIF: return "spdif";
    case DigitalAudioDisplayDevice: return "digital-display";
    case UnknownFormFactor: return "unknown";
    default: return "unrecognized";
    }
}

[[nodiscard]] std::string read_form_factor(IPropertyStore& properties) {
    PROPVARIANT value;
    PropVariantInit(&value);
    const auto code = properties.GetValue(kAudioEndpointFormFactor, &value);
    const auto* result = "unavailable";
    if (SUCCEEDED(code) && value.vt == VT_UI4) {
        result = form_factor_name(value.ulVal);
    }
    PropVariantClear(&value);
    return result;
}

[[nodiscard]] EndpointState endpoint_state(DWORD state) noexcept {
    if ((state & DEVICE_STATE_ACTIVE) != 0) {
        return EndpointState::Active;
    }
    if ((state & DEVICE_STATE_DISABLED) != 0) {
        return EndpointState::Disabled;
    }
    if ((state & DEVICE_STATE_NOTPRESENT) != 0) {
        return EndpointState::NotPresent;
    }
    if ((state & DEVICE_STATE_UNPLUGGED) != 0) {
        return EndpointState::Unplugged;
    }
    return EndpointState::Unknown;
}

} // namespace

bool probe_endpoint_metadata(
    IMMDevice& device,
    const std::wstring& endpoint_id,
    bool default_console,
    bool default_multimedia,
    bool default_communications,
    EndpointRecord& record,
    long& platform_code) {
    record.identity = stable_endpoint_hash(endpoint_id);
    DWORD raw_state = 0;
    auto code = device.GetState(&raw_state);
    if (FAILED(code)) {
        platform_code = code;
        return false;
    }
    record.state = endpoint_state(raw_state);
    record.default_console = default_console;
    record.default_multimedia = default_multimedia;
    record.default_communications = default_communications;

    ComPtr<IPropertyStore> properties;
    code = device.OpenPropertyStore(STGM_READ, properties.GetAddressOf());
    if (FAILED(code)) {
        platform_code = code;
        return false;
    }
    record.friendly_name =
        read_string(*properties.Get(), kDeviceFriendlyName);
    record.form_factor = read_form_factor(*properties.Get());

    const auto pnp = probe_pnp_device(endpoint_id);
    record.endpoint_driver_version = pnp.endpoint_driver_version;
    record.parent_device_name = pnp.parent_device_name;
    record.parent_driver_provider = pnp.parent_driver_provider;
    record.parent_driver_version = pnp.parent_driver_version;
    record.category = classify_device(pnp, record.form_factor);
    return true;
}

} // namespace device_matrix::windows
