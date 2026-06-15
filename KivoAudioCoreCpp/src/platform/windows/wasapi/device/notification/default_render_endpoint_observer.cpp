#include "platform/windows/wasapi/device/notification/default_render_endpoint_observer.hpp"

#include <cwchar>

namespace kivo::platform::windows::wasapi::detail {

DefaultRenderEndpointObserver::DefaultRenderEndpointObserver(
    const wchar_t* endpoint_id) noexcept {
    if (endpoint_id == nullptr) {
        return;
    }
    const auto length = std::wcslen(endpoint_id);
    if (length >= endpoint_id_.size()) {
        return;
    }
    std::wmemcpy(endpoint_id_.data(), endpoint_id, length + 1);
    endpoint_id_valid_ = true;
}

bool DefaultRenderEndpointObserver::is_valid() const noexcept {
    return endpoint_id_valid_;
}

bool DefaultRenderEndpointObserver::consume_change() noexcept {
    return change_pending_.exchange(false, std::memory_order_acq_rel);
}

uint64_t DefaultRenderEndpointObserver::change_count() const noexcept {
    return change_count_.load(std::memory_order_acquire);
}

HRESULT DefaultRenderEndpointObserver::QueryInterface(
    REFIID interface_id,
    void** object) noexcept {
    if (object == nullptr) {
        return E_POINTER;
    }
    *object = nullptr;
    if (interface_id == __uuidof(IUnknown)
        || interface_id == __uuidof(IMMNotificationClient)) {
        *object = static_cast<IMMNotificationClient*>(this);
        AddRef();
        return S_OK;
    }
    return E_NOINTERFACE;
}

ULONG DefaultRenderEndpointObserver::AddRef() noexcept {
    return reference_count_.fetch_add(1, std::memory_order_relaxed) + 1;
}

ULONG DefaultRenderEndpointObserver::Release() noexcept {
    const auto remaining =
        reference_count_.fetch_sub(1, std::memory_order_acq_rel) - 1;
    if (remaining == 0) {
        delete this;
    }
    return remaining;
}

HRESULT DefaultRenderEndpointObserver::OnDeviceStateChanged(
    LPCWSTR device_id,
    DWORD new_state) noexcept {
    if (is_current(device_id) && new_state != DEVICE_STATE_ACTIVE) {
        publish_change();
    }
    return S_OK;
}

HRESULT DefaultRenderEndpointObserver::OnDeviceAdded(LPCWSTR) noexcept {
    return S_OK;
}

HRESULT DefaultRenderEndpointObserver::OnDeviceRemoved(
    LPCWSTR device_id) noexcept {
    if (is_current(device_id)) {
        publish_change();
    }
    return S_OK;
}

HRESULT DefaultRenderEndpointObserver::OnDefaultDeviceChanged(
    EDataFlow flow,
    ERole role,
    LPCWSTR default_device_id) noexcept {
    if (flow == eRender
        && role == eConsole
        && !is_current(default_device_id)) {
        publish_change();
    }
    return S_OK;
}

HRESULT DefaultRenderEndpointObserver::OnPropertyValueChanged(
    LPCWSTR,
    const PROPERTYKEY) noexcept {
    return S_OK;
}

bool DefaultRenderEndpointObserver::is_current(
    const wchar_t* device_id) const noexcept {
    return endpoint_id_valid_
        && device_id != nullptr
        && std::wcscmp(endpoint_id_.data(), device_id) == 0;
}

void DefaultRenderEndpointObserver::publish_change() noexcept {
    change_count_.fetch_add(1, std::memory_order_relaxed);
    change_pending_.store(true, std::memory_order_release);
}

} // namespace kivo::platform::windows::wasapi::detail
