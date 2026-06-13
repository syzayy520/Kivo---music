#ifndef KIVO_HOST_ABI_VERSION_KIVO_AUDIO_ABI_VERSION_H
#define KIVO_HOST_ABI_VERSION_KIVO_AUDIO_ABI_VERSION_H

#include <stdint.h>

#define KIVO_AUDIO_ABI_VERSION_MAJOR 1u
#define KIVO_AUDIO_ABI_VERSION_MINOR 0u
#define KIVO_AUDIO_ABI_VERSION_PATCH 0u
#define KIVO_AUDIO_ABI_STRUCT_VERSION_1 1u

typedef struct kivo_audio_struct_header {
    uint32_t struct_size;
    uint32_t struct_version;
} kivo_audio_struct_header;

#endif
