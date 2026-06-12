#include "endpoint_probe.hpp"

#include <Windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <propsys.h>
#include <propvarutil.h>
#include <wrl/client.h>

#include <string>

namespace device_matrix::windows {

namespace {

using Microsoft::WRL::ComPtr;

constexpr PROPERTYKEY kDeviceFriendlyName{
    {0xa45c254e, 0xdf1c, 0x4efd,
     {0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0}},
    14
};
constexpr PROPERTYKEY kDeviceDriverVersion{
    {0xa8b865dd, 0x2e3d, 0x4094,
     {0xad, 0x97, 0xe5, 0x93, 0xa7, 0x0c, 0x75, 0xd6}},
    3
};
constexpr PROPERTYKEY kAudioEndpointFormFactor{
    {0x1da5d803, 0xd492, 0x4edd,
     {0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e}},
    0
};

[[nodiscard]] uint64_t stable_endpoint_hash(const wchar_t* value) noexcept {
    constexpr uint64_t offset_basis = 14695981039346656037ull;
    constexpr uint64_t prime = 1099511628211ull;
    uint64_t hash = offset_basis;
    while (value != nullptr && *value != L'\0') {
        hash ^= static_cast<uint16_t>(*value);
        hash *= prime;
        ++value;
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
    const auto written = WideCharToMultiByte(
        CP_UTF8,
        WC_ERR_INVALID_CHARS,
        value,
        -1,
        result.data(),
        required,
        nullptr,
        nullptr);
    if (written != required) {
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
    const auto code = properties.GetValue(
        kAudioEndpointFormFactor,
        &value);
    const auto* result = "unavailable";
    if (SUCCEEDED(code) && value.vt == VT_UI4) {
        result = form_factor_name(value.ulVal);
    }
    PropVariantClear(&value);
    return result;
}

} // namespace

bool probe_endpoint(
    IMMDevice& device,
    bool default_console,
    bool default_multimedia,
    bool default_communications,
    EndpointRecord& record,
    long& platform_code) {
    LPWSTR endpoint_id = nullptr;
    auto code = device.GetId(&endpoint_id);
    if (FAILED(code)) {
        platform_code = code;
        return false;
    }
    record.identity = stable_endpoint_hash(endpoint_id);
    CoTaskMemFree(endpoint_id);

    DWORD state = 0;
    code = device.GetState(&state);
    if (FAILED(code)) {
        platform_code = code;
        return false;
    }
    record.state = state;
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
    record.driver_version =
        read_string(*properties.Get(), kDeviceDriverVersion);
    record.form_factor = read_form_factor(*properties.Get());

    ComPtr<IAudioClient> client;
    code = device.Activate(
        __uuidof(IAudioClient),
        CLSCTX_INPROC_SERVER,
        nullptr,
        reinterpret_cast<void**>(client.GetAddressOf()));
    if (FAILED(code)) {
        platform_code = code;
        return false;
    }

    WAVEFORMATEX* mix_format = nullptr;
    code = client->GetMixFormat(&mix_format);
    if (FAILED(code) || mix_format == nullptr) {
        platform_code = FAILED(code) ? code : E_UNEXPECTED;
        return false;
    }
    record.mix_format = {
        mix_format->wFormatTag,
        mix_format->nChannels,
        mix_format->nSamplesPerSec,
        mix_format->wBitsPerSample,
        mix_format->nBlockAlign
    };

    WAVEFORMATEX* closest = nullptr;
    const auto shared_code = client->IsFormatSupported(
        AUDCLNT_SHAREMODE_SHARED,
        mix_format,
        &closest);
    record.shared_mix_supported = shared_code == S_OK;
    CoTaskMemFree(closest);
    record.exclusive_mix_supported = client->IsFormatSupported(
        AUDCLNT_SHAREMODE_EXCLUSIVE,
        mix_format,
        nullptr) == S_OK;

    code = client->GetDevicePeriod(
        &record.default_period_100ns,
        &record.minimum_period_100ns);
    CoTaskMemFree(mix_format);
    if (FAILED(code)) {
        platform_code = code;
        return false;
    }
    return true;
}

} // namespace device_matrix::windows
