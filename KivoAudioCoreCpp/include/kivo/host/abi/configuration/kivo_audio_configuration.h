#ifndef KIVO_HOST_ABI_CONFIGURATION_KIVO_AUDIO_CONFIGURATION_H
#define KIVO_HOST_ABI_CONFIGURATION_KIVO_AUDIO_CONFIGURATION_H

#include <stdint.h>

#include "kivo/host/abi/diagnostic/kivo_audio_diagnostic.h"
#include "kivo/host/abi/version/kivo_audio_abi_version.h"

typedef enum kivo_audio_sample_format {
    KIVO_AUDIO_SAMPLE_FORMAT_UNKNOWN = 0,
    KIVO_AUDIO_SAMPLE_FORMAT_INT16 = 1,
    KIVO_AUDIO_SAMPLE_FORMAT_INT24_IN_32 = 2,
    KIVO_AUDIO_SAMPLE_FORMAT_INT32 = 3,
    KIVO_AUDIO_SAMPLE_FORMAT_FLOAT32 = 4,
    KIVO_AUDIO_SAMPLE_FORMAT_FLOAT64 = 5
} kivo_audio_sample_format;

typedef enum kivo_audio_channel_layout {
    KIVO_AUDIO_CHANNEL_LAYOUT_UNKNOWN = 0,
    KIVO_AUDIO_CHANNEL_LAYOUT_MONO = 1,
    KIVO_AUDIO_CHANNEL_LAYOUT_STEREO = 2,
    KIVO_AUDIO_CHANNEL_LAYOUT_SURROUND_5_1 = 3,
    KIVO_AUDIO_CHANNEL_LAYOUT_SURROUND_7_1 = 4
} kivo_audio_channel_layout;

typedef enum kivo_audio_output_mode {
    KIVO_AUDIO_OUTPUT_MODE_SHARED = 0,
    KIVO_AUDIO_OUTPUT_MODE_EXCLUSIVE = 1
} kivo_audio_output_mode;

typedef enum kivo_audio_resample_quality {
    KIVO_AUDIO_RESAMPLE_QUALITY_FAST = 0,
    KIVO_AUDIO_RESAMPLE_QUALITY_BALANCED = 1,
    KIVO_AUDIO_RESAMPLE_QUALITY_HIGH = 2
} kivo_audio_resample_quality;

typedef enum kivo_audio_dither_policy {
    KIVO_AUDIO_DITHER_DISABLED = 0,
    KIVO_AUDIO_DITHER_TRIANGULAR_PDF = 1
} kivo_audio_dither_policy;

typedef struct kivo_audio_create_info_v1 {
    uint32_t struct_size;
    uint32_t struct_version;
    void* diagnostic_context;
    kivo_audio_diagnostic_callback diagnostic_callback;
    uint64_t reserved[4];
} kivo_audio_create_info_v1;

typedef struct kivo_audio_open_configuration_v1 {
    uint32_t struct_size;
    uint32_t struct_version;
    uint32_t sample_format;
    uint32_t channel_layout;
    uint32_t sample_rate;
    uint32_t output_mode;
    uint32_t requested_buffer_frames;
    uint32_t queue_slot_count;
    uint32_t maximum_chunk_frames;
    uint32_t allow_conversion;
    uint32_t resample_quality;
    uint32_t conversion_dither;
    uint32_t bit_perfect_required;
    uint32_t replay_gain_enabled;
    double replay_gain_db;
    double replay_gain_peak;
    uint32_t replay_gain_peak_known;
    uint32_t prevent_clipping;
    uint32_t software_volume_enabled;
    double software_volume_linear_gain;
    uint32_t processing_dither;
    uint32_t reserved;
    uint64_t dither_seed;
} kivo_audio_open_configuration_v1;

#endif
