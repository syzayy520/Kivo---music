#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include <objbase.h>

#include <new>

namespace kivo::platform::windows::wasapi::detail {

namespace {

[[nodiscard]] uint64_t stable_endpoint_hash(const wchar_t* value) noexcept {
    constexpr uint64_t offset_basis = 14695981039346656037ull;
    constexpr uint64_t prime = 1099511628211ull;
    uint64_t hash = offset_basis;
    if (value != nullptr) {
        while (*value != L'\0') {
            hash ^= static_cast<uint16_t>(*value);
            hash *= prime;
            ++value;
        }
    }
    return hash == 0 ? 1 : hash;
}

} // namespace

HRESULT WasapiRendererState::prepare_open_environment(
    uint64_t& endpoint_identity,
    REFERENCE_TIME& default_device_period,
    REFERENCE_TIME& minimum_device_period) noexcept {
    endpoint_identity = 0;
    default_device_period = 0;
    minimum_device_period = 0;

    auto code = apartment_.initialize();
    if (FAILED(code)) {
        return code;
    }
    code = CoCreateInstance(
        __uuidof(MMDeviceEnumerator),
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&enumerator_));
    if (FAILED(code)) {
        return code;
    }
    code = enumerator_->GetDefaultAudioEndpoint(eRender, eConsole, &device_);
    if (FAILED(code)) {
        return code;
    }

    LPWSTR endpoint_id = nullptr;
    code = device_->GetId(&endpoint_id);
    if (FAILED(code)) {
        return code;
    }
    endpoint_identity = stable_endpoint_hash(endpoint_id);
    endpoint_observer_.Attach(
        new (std::nothrow) DefaultRenderEndpointObserver(endpoint_id));
    CoTaskMemFree(endpoint_id);
    if (endpoint_observer_ == nullptr || !endpoint_observer_->is_valid()) {
        return E_OUTOFMEMORY;
    }

    code = enumerator_->RegisterEndpointNotificationCallback(
        endpoint_observer_.Get());
    if (FAILED(code)) {
        return code;
    }
    endpoint_observer_registered_ = true;

    const auto power_code = power_observer_.register_callback();
    if (power_code != ERROR_SUCCESS) {
        return HRESULT_FROM_WIN32(power_code);
    }
    code = device_->Activate(
        __uuidof(IAudioClient),
        CLSCTX_INPROC_SERVER,
        nullptr,
        reinterpret_cast<void**>(audio_client_.GetAddressOf()));
    if (FAILED(code)) {
        return code;
    }
    code = audio_client_->GetDevicePeriod(
        &default_device_period,
        &minimum_device_period);
    if (FAILED(code)) {
        return code;
    }
    if (default_device_period <= 0 || minimum_device_period <= 0) {
        return E_UNEXPECTED;
    }

    event_handle_ = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    return event_handle_ != nullptr
        ? S_OK
        : HRESULT_FROM_WIN32(GetLastError());
}

} // namespace kivo::platform::windows::wasapi::detail
