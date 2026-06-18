#pragma once

#include <memory>
#include <mutex>

#include "kivo/adapters/ffmpeg/session/ffmpeg_audio_decode_session.hpp"
#include "kivo/core/playback/runtime/coordinator/playback_runtime_coordinator.hpp"
#include "kivo/platform/windows/wasapi/renderer/wasapi_renderer.hpp"
#include "kivo/host/abi/api/kivo_audio_api.h"

namespace kivo::host::abi {

class HostAudioEngine final {
public:
    explicit HostAudioEngine(
        const kivo_audio_create_info_v1& create_info);
    ~HostAudioEngine();

    HostAudioEngine(const HostAudioEngine&) = delete;
    HostAudioEngine& operator=(const HostAudioEngine&) = delete;

    [[nodiscard]] kivo_audio_result capabilities(
        kivo_audio_capabilities_v1& capabilities) noexcept;
    [[nodiscard]] kivo_audio_result open_source(
        uint64_t command_id,
        uint64_t session_generation,
        std::unique_ptr<core::decode::ByteSourceBoundary> source,
        const kivo_audio_open_configuration_v1& configuration) noexcept;
    [[nodiscard]] kivo_audio_result submit(
        const kivo_audio_command_v1& command) noexcept;
    [[nodiscard]] kivo_audio_result pump(
        const kivo_audio_pump_request_v1& request,
        kivo_audio_pump_result_v1& result) noexcept;
    [[nodiscard]] kivo_audio_result snapshot(
        void* snapshot,
        uint32_t snapshot_size) noexcept;
    [[nodiscard]] kivo_audio_result truth(
        void* truth,
        uint32_t truth_size) noexcept;
    void shutdown_for_destroy() noexcept;

private:
    void emit(
        kivo_audio_result result,
        kivo_audio_diagnostic_domain domain,
        uint64_t command_id) noexcept;
    [[nodiscard]] uint64_t next_internal_command_id() const noexcept;

    mutable std::mutex command_mutex_;
    core::playback::PlaybackSessionController session_;
    adapters::ffmpeg::FfmpegAudioDecodeSession decoder_;
    platform::windows::wasapi::WasapiRenderer renderer_;
    core::playback::PlaybackRuntimeCoordinator runtime_;
    void* diagnostic_context_{nullptr};
    kivo_audio_diagnostic_callback diagnostic_callback_{nullptr};
    uint64_t diagnostic_sequence_{0};
    kivo_audio_result last_result_{KIVO_AUDIO_RESULT_OK};
};

} // namespace kivo::host::abi
