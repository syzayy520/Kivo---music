#ifndef KIVO_HOST_ABI_SNAPSHOT_KIVO_AUDIO_SNAPSHOT_H
#define KIVO_HOST_ABI_SNAPSHOT_KIVO_AUDIO_SNAPSHOT_H

#include <stdint.h>

#include "kivo/host/abi/result/kivo_audio_result.h"
#include "kivo/host/abi/version/kivo_audio_abi_version.h"

typedef enum kivo_audio_state {
    KIVO_AUDIO_STATE_UNKNOWN = 0,
    KIVO_AUDIO_STATE_IDLE = 1,
    KIVO_AUDIO_STATE_READY = 2,
    KIVO_AUDIO_STATE_PLAYING = 3,
    KIVO_AUDIO_STATE_PAUSED = 4,
    KIVO_AUDIO_STATE_SEEKING = 5,
    KIVO_AUDIO_STATE_DRAINING = 6,
    KIVO_AUDIO_STATE_RECOVERING = 7,
    KIVO_AUDIO_STATE_STOPPED = 8,
    KIVO_AUDIO_STATE_FAILED = 9,
    KIVO_AUDIO_STATE_CLOSED = 10
} kivo_audio_state;

typedef struct kivo_audio_snapshot_v1 {
    uint32_t struct_size;
    uint32_t struct_version;
    uint32_t state;
    uint32_t active;
    uint64_t session_generation;
    uint64_t last_command_id;
    uint64_t rendered_position_frames;
    uint64_t decoded_frames;
    uint64_t queued_frames;
    uint64_t successful_opens;
    uint64_t successful_seeks;
    uint64_t failed_operations;
    uint64_t stale_blocks_rejected;
    uint64_t device_loss_events;
    uint64_t decode_failure_events;
    kivo_audio_result last_result;
    uint32_t reserved;
} kivo_audio_snapshot_v1;

#endif
