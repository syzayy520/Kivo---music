#ifndef KIVO_HOST_ABI_SOURCE_KIVO_AUDIO_SOURCE_H
#define KIVO_HOST_ABI_SOURCE_KIVO_AUDIO_SOURCE_H

#include <stdint.h>

#include "kivo/host/abi/version/kivo_audio_abi_version.h"

typedef enum kivo_audio_source_io_result {
    KIVO_AUDIO_SOURCE_IO_SUCCESS = 0,
    KIVO_AUDIO_SOURCE_IO_DATA = KIVO_AUDIO_SOURCE_IO_SUCCESS,
    KIVO_AUDIO_SOURCE_IO_END_OF_STREAM = 1,
    KIVO_AUDIO_SOURCE_IO_UNSUPPORTED = 2,
    KIVO_AUDIO_SOURCE_IO_FAILED = 3
} kivo_audio_source_io_result;

typedef enum kivo_audio_seek_origin {
    KIVO_AUDIO_SEEK_ORIGIN_BEGIN = 0,
    KIVO_AUDIO_SEEK_ORIGIN_CURRENT = 1,
    KIVO_AUDIO_SEEK_ORIGIN_END = 2
} kivo_audio_seek_origin;

typedef enum kivo_audio_source_flag {
    KIVO_AUDIO_SOURCE_FLAG_SEEKABLE = 1u << 0u,
    KIVO_AUDIO_SOURCE_FLAG_SIZE_KNOWN = 1u << 1u
} kivo_audio_source_flag;

typedef kivo_audio_source_io_result (*kivo_audio_source_read_callback)(
    void* source_context,
    uint8_t* destination,
    uint64_t capacity_bytes,
    uint64_t* bytes_read);

typedef kivo_audio_source_io_result (*kivo_audio_source_seek_callback)(
    void* source_context,
    int64_t offset,
    kivo_audio_seek_origin origin,
    uint64_t* absolute_position);

typedef void (*kivo_audio_source_release_callback)(void* source_context);

typedef struct kivo_audio_source_v1 {
    uint32_t struct_size;
    uint32_t struct_version;
    void* source_context;
    uint64_t source_identity;
    uint64_t source_generation;
    uint64_t size_bytes;
    uint32_t flags;
    uint32_t reserved;
    kivo_audio_source_read_callback read;
    kivo_audio_source_seek_callback seek;
    kivo_audio_source_release_callback release;
} kivo_audio_source_v1;

#endif
