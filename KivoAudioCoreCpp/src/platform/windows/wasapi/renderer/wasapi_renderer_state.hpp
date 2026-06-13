#pragma once

#include <Windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <wrl/client.h>

#include "kivo/platform/windows/wasapi/renderer/wasapi_renderer.hpp"
#include "platform/windows/wasapi/apartment/com_apartment.hpp"
#include "platform/windows/wasapi/device/notification/default_render_endpoint_observer.hpp"
#include "platform/windows/wasapi/power/notification/audio_power_transition_observer.hpp"

namespace kivo::platform::windows::wasapi::detail {

struct WasapiStreamOpenResult;

class WasapiRendererState final {
public:
    ~WasapiRendererState();

    [[nodiscard]] core::render::RendererIdentity identity() const noexcept;
    [[nodiscard]] core::render::RendererCapabilitySnapshot capabilities() const noexcept;
    [[nodiscard]] core::render::RenderOpenResult open(
        const core::render::RenderOpenRequest& request) noexcept;
    [[nodiscard]] core::render::RenderControlResult start() noexcept;
    [[nodiscard]] core::render::RenderControlResult flush(
        const core::render::RenderFlushRequest& request) noexcept;
    [[nodiscard]] core::render::RenderControlResult drain(
        const core::render::RenderDrainRequest& request) noexcept;
    [[nodiscard]] core::render::RenderControlResult reset(
        const core::render::RenderResetRequest& request) noexcept;
    [[nodiscard]] core::render::RenderControlResult stop() noexcept;
    [[nodiscard]] core::render::RenderControlResult close() noexcept;
    [[nodiscard]] core::render::RenderSnapshot snapshot() const noexcept;
    [[nodiscard]] core::render::RenderWriteResult write(
        const core::render::AudioBlockView& block) noexcept;
    [[nodiscard]] WasapiWaitResult wait_until_writable(
        std::chrono::milliseconds timeout) noexcept;
    [[nodiscard]] WasapiDiagnostics diagnostics() const noexcept;

private:
    [[nodiscard]] bool on_control_thread() const noexcept;
    [[nodiscard]] core::render::RenderControlResult wrong_thread_result() noexcept;
    [[nodiscard]] core::render::RenderFailure stale_generation(
        const core::render::RenderGenerationSet& generations) const noexcept;
    [[nodiscard]] bool detect_render_environment_change() noexcept;
    [[nodiscard]] HRESULT prepare_open_environment(
        uint64_t& endpoint_identity,
        REFERENCE_TIME& default_device_period,
        REFERENCE_TIME& minimum_device_period) noexcept;
    [[nodiscard]] HRESULT initialize_shared_stream(
        const core::render::RenderOpenRequest& request,
        core::contract::RenderFormat& accepted_format) noexcept;
    [[nodiscard]] HRESULT initialize_exclusive_stream(
        const core::render::RenderOpenRequest& request,
        REFERENCE_TIME minimum_device_period,
        core::contract::RenderFormat& accepted_format) noexcept;
    [[nodiscard]] HRESULT acquire_render_service(
        core::contract::FrameCount& buffer_frames) noexcept;
    [[nodiscard]] core::render::RenderOpenResult complete_open(
        const core::render::RenderOpenRequest& request,
        const WasapiStreamOpenResult& stream,
        uint64_t endpoint_identity,
        REFERENCE_TIME default_device_period,
        REFERENCE_TIME minimum_device_period) noexcept;
    [[nodiscard]] core::render::RenderOpenResult fail_open(HRESULT result) noexcept;
    [[nodiscard]] core::render::RenderControlResult fail_control(HRESULT result) noexcept;
    [[nodiscard]] core::render::RenderWriteResult fail_write(
        HRESULT result,
        core::contract::FrameCount submitted_frames) noexcept;
    void close_stream() noexcept;

    ComApartment apartment_;
    Microsoft::WRL::ComPtr<IMMDeviceEnumerator> enumerator_;
    Microsoft::WRL::ComPtr<DefaultRenderEndpointObserver> endpoint_observer_;
    Microsoft::WRL::ComPtr<IMMDevice> device_;
    Microsoft::WRL::ComPtr<IAudioClient> audio_client_;
    Microsoft::WRL::ComPtr<IAudioRenderClient> render_client_;
    AudioPowerTransitionObserver power_observer_;
    HANDLE event_handle_{nullptr};
    DWORD control_thread_id_{0};
    core::contract::DeviceGeneration device_generation_{};
    bool opened_before_{false};
    bool endpoint_observer_registered_{false};
    bool has_submitted_since_start_{false};
    core::render::RendererCapabilitySnapshot capabilities_{};
    core::render::RenderSnapshot snapshot_{};
    WasapiDiagnostics diagnostics_{};
};

} // namespace kivo::platform::windows::wasapi::detail
