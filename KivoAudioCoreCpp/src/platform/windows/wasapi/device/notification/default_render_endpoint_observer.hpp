#pragma once

#include <Windows.h>
#include <mmdeviceapi.h>

#include <array>
#include <atomic>
#include <cstdint>

namespace kivo::platform::windows::wasapi::detail {

class DefaultRenderEndpointObserver final : public IMMNotificationClient {
public:
    explicit DefaultRenderEndpointObserver(const wchar_t* endpoint_id) noexcept;

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool consume_change() noexcept;
    [[nodiscard]] uint64_t change_count() const noexcept;

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID interface_id,
        void** object) noexcept override;
    ULONG STDMETHODCALLTYPE AddRef() noexcept override;
    ULONG STDMETHODCALLTYPE Release() noexcept override;
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(
        LPCWSTR device_id,
        DWORD new_state) noexcept override;
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR device_id) noexcept override;
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR device_id) noexcept override;
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(
        EDataFlow flow,
        ERole role,
        LPCWSTR default_device_id) noexcept override;
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(
        LPCWSTR device_id,
        const PROPERTYKEY key) noexcept override;

private:
    [[nodiscard]] bool is_current(const wchar_t* device_id) const noexcept;
    void publish_change() noexcept;

    static constexpr size_t kMaximumEndpointIdCharacters = 512;
    std::atomic<ULONG> reference_count_{1};
    std::atomic<bool> change_pending_{false};
    std::atomic<uint64_t> change_count_{0};
    std::array<wchar_t, kMaximumEndpointIdCharacters> endpoint_id_{};
    bool endpoint_id_valid_{false};
};

} // namespace kivo::platform::windows::wasapi::detail
