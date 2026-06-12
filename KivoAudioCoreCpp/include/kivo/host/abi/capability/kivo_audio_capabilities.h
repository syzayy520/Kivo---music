#ifndef KIVO_HOST_ABI_CAPABILITY_KIVO_AUDIO_CAPABILITIES_H
#define KIVO_HOST_ABI_CAPABILITY_KIVO_AUDIO_CAPABILITIES_H

#include <stdint.h>

#include "kivo/host/abi/version/kivo_audio_abi_version.h"

typedef enum kivo_audio_capability_flag {
    KIVO_AUDIO_CAPABILITY_HOST_SOURCE_CALLBACKS = 1ull << 0u,
    KIVO_AUDIO_CAPABILITY_SEEK = 1ull << 1u,
    KIVO_AUDIO_CAPABILITY_SHARED_OUTPUT = 1ull << 2u,
    KIVO_AUDIO_CAPABILITY_EXCLUSIVE_OUTPUT = 1ull << 3u,
    KIVO_AUDIO_CAPABILITY_REPLAY_GAIN = 1ull << 4u,
    KIVO_AUDIO_CAPABILITY_SOFTWARE_VOLUME = 1ull << 5u,
    KIVO_AUDIO_CAPABILITY_DITHER = 1ull << 6u,
    KIVO_AUDIO_CAPABILITY_SANITIZED_DIAGNOSTICS = 1ull << 7u
} kivo_audio_capability_flag;

typedef enum kivo_audio_thread_rule_flag {
    KIVO_AUDIO_THREAD_RULE_SERIALIZE_COMMANDS = 1u << 0u,
    KIVO_AUDIO_THREAD_RULE_SOURCE_ON_CALLER = 1u << 1u,
    KIVO_AUDIO_THREAD_RULE_DIAGNOSTIC_ON_CALLER = 1u << 2u,
    KIVO_AUDIO_THREAD_RULE_NO_RENDER_CALLBACKS = 1u << 3u
} kivo_audio_thread_rule_flag;

typedef struct kivo_audio_capabilities_v1 {
    uint32_t struct_size;
    uint32_t struct_version;
    uint32_t abi_major;
    uint32_t abi_minor;
    uint32_t abi_patch;
    uint32_t maximum_struct_version;
    uint64_t capability_flags;
    uint32_t thread_rule_flags;
    uint32_t maximum_queue_slots;
    uint32_t maximum_sample_rate;
    uint32_t maximum_channels;
    uint64_t reserved[4];
} kivo_audio_capabilities_v1;

#endif
