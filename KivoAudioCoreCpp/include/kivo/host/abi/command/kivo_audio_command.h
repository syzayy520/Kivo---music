#ifndef KIVO_HOST_ABI_COMMAND_KIVO_AUDIO_COMMAND_H
#define KIVO_HOST_ABI_COMMAND_KIVO_AUDIO_COMMAND_H

#include <stdint.h>

#include "kivo/host/abi/version/kivo_audio_abi_version.h"

typedef enum kivo_audio_command_kind {
    KIVO_AUDIO_COMMAND_RESUME = 1,
    KIVO_AUDIO_COMMAND_PAUSE = 2,
    KIVO_AUDIO_COMMAND_STOP = 3,
    KIVO_AUDIO_COMMAND_SEEK = 4,
    KIVO_AUDIO_COMMAND_FLUSH = 5,
    KIVO_AUDIO_COMMAND_CLOSE_SOURCE = 6,
    KIVO_AUDIO_COMMAND_SHUTDOWN = 7,
    /* ABI 1.1.0: runtime software-volume change. */
    KIVO_AUDIO_COMMAND_SET_VOLUME = 8
} kivo_audio_command_kind;

/* For KIVO_AUDIO_COMMAND_SET_VOLUME ONLY, the command's `requested_frame` field
   carries the linear volume gain as Q40.24 fixed point (gain * (1 << 24)); the
   valid range is [0, KIVO_AUDIO_VOLUME_GAIN_FIXED_ONE] == [0.0, 1.0]. For every
   other command `requested_frame` keeps its usual meaning (an absolute PCM frame
   position for SEEK). The command struct layout is unchanged (additive ABI). */
#define KIVO_AUDIO_VOLUME_GAIN_FIXED_ONE 16777216u  /* 1 << 24 */

typedef struct kivo_audio_command_v1 {
    uint32_t struct_size;
    uint32_t struct_version;
    uint64_t command_id;
    uint64_t session_generation;
    uint32_t kind;
    uint32_t reserved;
    uint64_t requested_frame;
} kivo_audio_command_v1;

typedef struct kivo_audio_pump_request_v1 {
    uint32_t struct_size;
    uint32_t struct_version;
    uint32_t maximum_steps;
    uint32_t reserved;
} kivo_audio_pump_request_v1;

typedef struct kivo_audio_pump_result_v1 {
    uint32_t struct_size;
    uint32_t struct_version;
    uint32_t steps_executed;
    uint32_t made_progress;
    uint32_t reached_end_of_stream;
    uint32_t backpressured;
    uint64_t produced_frames;
    uint64_t rendered_frames;
} kivo_audio_pump_result_v1;

#endif
