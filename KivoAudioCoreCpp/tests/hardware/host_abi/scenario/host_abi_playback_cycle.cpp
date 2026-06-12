#include "host_abi_playback_cycle.hpp"

#include "../fixture/local_callback_source.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#include "kivo/host/abi/api/kivo_audio_api.h"

namespace kivo::hardware_test::host_abi {

namespace {

template <typename Structure>
[[nodiscard]] Structure abi_structure() noexcept {
    Structure result{};
    result.struct_size = sizeof(result);
    result.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    return result;
}

} // namespace

int run_host_abi_playback(const std::filesystem::path& path) {
    auto create = abi_structure<kivo_audio_create_info_v1>();
    kivo_audio_handle handle = KIVO_AUDIO_INVALID_HANDLE;
    if (kivo_audio_create(&create, &handle)
        != KIVO_AUDIO_RESULT_OK) {
        return 2;
    }

    auto source = make_local_callback_source(path);
    if (source.read == nullptr) {
        static_cast<void>(kivo_audio_destroy(handle));
        return 3;
    }
    auto configuration =
        abi_structure<kivo_audio_open_configuration_v1>();
    configuration.sample_format = KIVO_AUDIO_SAMPLE_FORMAT_FLOAT32;
    configuration.channel_layout = KIVO_AUDIO_CHANNEL_LAYOUT_STEREO;
    configuration.sample_rate = 48000;
    configuration.output_mode = KIVO_AUDIO_OUTPUT_MODE_SHARED;
    configuration.requested_buffer_frames = 4800;
    configuration.queue_slot_count = 64;
    configuration.maximum_chunk_frames = 960;
    configuration.allow_conversion = 1;
    configuration.resample_quality =
        KIVO_AUDIO_RESAMPLE_QUALITY_BALANCED;
    configuration.prevent_clipping = 1;
    configuration.software_volume_linear_gain = 1.0;
    configuration.dither_seed = 1;
    const auto opened = kivo_audio_open_source(
        handle,
        1,
        1,
        &source,
        &configuration);
    if (opened != KIVO_AUDIO_RESULT_OK) {
        std::cerr << "ABI open failed: " << opened << '\n';
        static_cast<void>(kivo_audio_destroy(handle));
        return 4;
    }

    auto pump_request =
        abi_structure<kivo_audio_pump_request_v1>();
    pump_request.maximum_steps = 64;
    auto pump_result =
        abi_structure<kivo_audio_pump_result_v1>();
    static_cast<void>(
        kivo_audio_pump(handle, &pump_request, &pump_result));

    auto resume = abi_structure<kivo_audio_command_v1>();
    resume.command_id = 2;
    resume.session_generation = 1;
    resume.kind = KIVO_AUDIO_COMMAND_RESUME;
    if (kivo_audio_submit_command(handle, &resume)
        != KIVO_AUDIO_RESULT_OK) {
        static_cast<void>(kivo_audio_destroy(handle));
        return 5;
    }

    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::minutes{10};
    uint64_t rendered_frames = 0;
    while (std::chrono::steady_clock::now() < deadline) {
        pump_result =
            abi_structure<kivo_audio_pump_result_v1>();
        const auto result =
            kivo_audio_pump(handle, &pump_request, &pump_result);
        if (result != KIVO_AUDIO_RESULT_OK) {
            std::cerr << "ABI pump failed: " << result << '\n';
            static_cast<void>(kivo_audio_destroy(handle));
            return 6;
        }
        rendered_frames += pump_result.rendered_frames;
        if (pump_result.reached_end_of_stream != 0u) {
            std::cout << "host_abi_rendered_frames="
                      << rendered_frames << '\n';
            static_cast<void>(kivo_audio_destroy(handle));
            return 0;
        }
        if (pump_result.made_progress == 0u) {
            std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }
    }
    static_cast<void>(kivo_audio_destroy(handle));
    return 7;
}

} // namespace kivo::hardware_test::host_abi
