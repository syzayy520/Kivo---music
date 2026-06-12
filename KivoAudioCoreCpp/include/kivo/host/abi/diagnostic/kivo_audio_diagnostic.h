#ifndef KIVO_HOST_ABI_DIAGNOSTIC_KIVO_AUDIO_DIAGNOSTIC_H
#define KIVO_HOST_ABI_DIAGNOSTIC_KIVO_AUDIO_DIAGNOSTIC_H

#include <stdint.h>

#include "kivo/host/abi/result/kivo_audio_result.h"
#include "kivo/host/abi/version/kivo_audio_abi_version.h"

typedef enum kivo_audio_diagnostic_domain {
    KIVO_AUDIO_DIAGNOSTIC_DOMAIN_HOST = 0,
    KIVO_AUDIO_DIAGNOSTIC_DOMAIN_SOURCE = 1,
    KIVO_AUDIO_DIAGNOSTIC_DOMAIN_DECODE = 2,
    KIVO_AUDIO_DIAGNOSTIC_DOMAIN_RENDER = 3,
    KIVO_AUDIO_DIAGNOSTIC_DOMAIN_LIFECYCLE = 4
} kivo_audio_diagnostic_domain;

typedef struct kivo_audio_diagnostic_event_v1 {
    uint32_t struct_size;
    uint32_t struct_version;
    uint64_t sequence;
    uint64_t command_id;
    uint32_t domain;
    uint32_t code;
    kivo_audio_result result;
    uint32_t reserved;
} kivo_audio_diagnostic_event_v1;

typedef void (*kivo_audio_diagnostic_callback)(
    void* user_context,
    const kivo_audio_diagnostic_event_v1* event);

#endif
