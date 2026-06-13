#pragma once

#include "kivo/host/abi/api/kivo_audio_api.h"

namespace host_abi_test {

[[nodiscard]] inline kivo_audio_create_info_v1 create_info() noexcept {
    kivo_audio_create_info_v1 result{};
    result.struct_size = sizeof(result);
    result.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    return result;
}

template <typename Structure>
[[nodiscard]] inline Structure output_structure() noexcept {
    Structure result{};
    result.struct_size = sizeof(result);
    result.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    return result;
}

[[nodiscard]] inline kivo_audio_open_configuration_v1
invalid_open_configuration() noexcept {
    auto result =
        output_structure<kivo_audio_open_configuration_v1>();
    result.sample_format = KIVO_AUDIO_SAMPLE_FORMAT_FLOAT32;
    result.channel_layout = KIVO_AUDIO_CHANNEL_LAYOUT_STEREO;
    result.sample_rate = 48000;
    result.output_mode = KIVO_AUDIO_OUTPUT_MODE_SHARED;
    result.requested_buffer_frames = 4800;
    result.queue_slot_count = 1;
    result.maximum_chunk_frames = 480;
    result.resample_quality =
        KIVO_AUDIO_RESAMPLE_QUALITY_BALANCED;
    result.software_volume_linear_gain = 1.0;
    result.prevent_clipping = 1;
    result.dither_seed = 1;
    return result;
}

} // namespace host_abi_test
