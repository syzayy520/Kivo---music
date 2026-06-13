#include "pnp_device_probe.hpp"

#include <Windows.h>
#include <initguid.h>
#include <devpkey.h>
#include <setupapi.h>

#include <array>
#include <cwctype>
#include <utility>

namespace device_matrix::windows {

namespace {

class DeviceInfoSet final {
public:
    DeviceInfoSet() noexcept
        : handle_(SetupDiGetClassDevsW(
            nullptr,
            nullptr,
            nullptr,
            DIGCF_ALLCLASSES)) {}

    ~DeviceInfoSet() {
        if (handle_ != INVALID_HANDLE_VALUE) {
            SetupDiDestroyDeviceInfoList(handle_);
        }
    }

    [[nodiscard]] HDEVINFO get() const noexcept {
        return handle_;
    }

private:
    HDEVINFO handle_{INVALID_HANDLE_VALUE};
};

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

[[nodiscard]] std::wstring read_wide_property(
    HDEVINFO set,
    SP_DEVINFO_DATA& device,
    const DEVPROPKEY& key) {
    std::array<wchar_t, 1024> value{};
    DEVPROPTYPE type = 0;
    DWORD required = 0;
    if (!SetupDiGetDevicePropertyW(
            set,
            &device,
            &key,
            &type,
            reinterpret_cast<PBYTE>(value.data()),
            static_cast<DWORD>(sizeof(value)),
            &required,
            0)
        || (type != DEVPROP_TYPE_STRING
            && type != DEVPROP_TYPE_STRING_INDIRECT)) {
        return {};
    }
    return value.data();
}

[[nodiscard]] bool open_device(
    HDEVINFO set,
    const std::wstring& instance_id,
    SP_DEVINFO_DATA& device) noexcept {
    device = {};
    device.cbSize = sizeof(device);
    return SetupDiOpenDeviceInfoW(
        set,
        instance_id.c_str(),
        nullptr,
        0,
        &device) != FALSE;
}

[[nodiscard]] std::wstring uppercase(std::wstring value) {
    for (auto& character : value) {
        character = static_cast<wchar_t>(std::towupper(character));
    }
    return value;
}

[[nodiscard]] bool starts_with(
    const std::wstring& value,
    const wchar_t* prefix) noexcept {
    return value.rfind(prefix, 0) == 0;
}

} // namespace

PnpDeviceRecord probe_pnp_device(const std::wstring& endpoint_id) {
    PnpDeviceRecord result;
    DeviceInfoSet devices;
    if (devices.get() == INVALID_HANDLE_VALUE) {
        return result;
    }

    const auto endpoint_instance_id =
        std::wstring{L"SWD\\MMDEVAPI\\"} + endpoint_id;
    SP_DEVINFO_DATA endpoint{};
    if (!open_device(devices.get(), endpoint_instance_id, endpoint)) {
        return result;
    }

    result.endpoint_driver_version = utf8(read_wide_property(
        devices.get(),
        endpoint,
        DEVPKEY_Device_DriverVersion).c_str());
    result.parent_instance_id = read_wide_property(
        devices.get(),
        endpoint,
        DEVPKEY_Device_Parent);
    if (result.parent_instance_id.empty()) {
        return result;
    }

    SP_DEVINFO_DATA parent{};
    if (!open_device(devices.get(), result.parent_instance_id, parent)) {
        return result;
    }
    result.parent_device_name = utf8(read_wide_property(
        devices.get(),
        parent,
        DEVPKEY_Device_FriendlyName).c_str());
    if (result.parent_device_name.empty()) {
        result.parent_device_name = utf8(read_wide_property(
            devices.get(),
            parent,
            DEVPKEY_Device_DeviceDesc).c_str());
    }
    result.parent_driver_provider = utf8(read_wide_property(
        devices.get(),
        parent,
        DEVPKEY_Device_DriverProvider).c_str());
    result.parent_driver_version = utf8(read_wide_property(
        devices.get(),
        parent,
        DEVPKEY_Device_DriverVersion).c_str());
    return result;
}

DeviceCategory classify_device(
    const PnpDeviceRecord& device,
    const std::string& form_factor) {
    if (form_factor == "digital-display") {
        return DeviceCategory::DisplayAudio;
    }
    const auto parent = uppercase(device.parent_instance_id);
    if (starts_with(parent, L"USB\\")) {
        return DeviceCategory::Usb;
    }
    if (starts_with(parent, L"BTH")
        || starts_with(parent, L"BLUETOOTH\\")) {
        return DeviceCategory::Bluetooth;
    }
    if (starts_with(parent, L"HDAUDIO\\")) {
        return DeviceCategory::Integrated;
    }
    return parent.empty()
        ? DeviceCategory::Unknown
        : DeviceCategory::Other;
}

} // namespace device_matrix::windows
