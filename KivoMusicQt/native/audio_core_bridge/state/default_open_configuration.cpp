// =============================================================================
// Kivo Music Qt - default_open_configuration.cpp
// AudioCoreBridge: Default Host ABI open configuration
// =============================================================================

#include "default_open_configuration.hpp"

namespace kivo::qt::audio_bridge::state {

kivo_audio_open_configuration_v1 makeDefaultOpenConfiguration() noexcept {
    kivo_audio_open_configuration_v1 configuration{};
    configuration.struct_size = sizeof(kivo_audio_open_configuration_v1);
    configuration.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    configuration.sample_format = KIVO_AUDIO_SAMPLE_FORMAT_FLOAT32;
    configuration.channel_layout = KIVO_AUDIO_CHANNEL_LAYOUT_STEREO;
    configuration.sample_rate = 48000;
    configuration.output_mode = KIVO_AUDIO_OUTPUT_MODE_SHARED;
    configuration.requested_buffer_frames = 960;
    configuration.queue_slot_count = 8;
    configuration.maximum_chunk_frames = 1024;
    configuration.allow_conversion = 1;
    configuration.resample_quality = KIVO_AUDIO_RESAMPLE_QUALITY_BALANCED;
    configuration.conversion_dither = KIVO_AUDIO_DITHER_DISABLED;
    configuration.bit_perfect_required = 0;
    configuration.replay_gain_enabled = 0;
    configuration.replay_gain_db = 0.0;
    configuration.replay_gain_peak = 1.0;
    configuration.replay_gain_peak_known = 0;
    configuration.prevent_clipping = 1;
    configuration.software_volume_enabled = 1;
    configuration.software_volume_linear_gain = 0.7;
    configuration.processing_dither = KIVO_AUDIO_DITHER_DISABLED;
    configuration.dither_seed = 0x4b49564f4d555349ULL;
    return configuration;
}

} // namespace kivo::qt::audio_bridge::state
