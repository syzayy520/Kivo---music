#ifndef KIVO_HOST_ABI_SNAPSHOT_KIVO_AUDIO_SNAPSHOT_H
#define KIVO_HOST_ABI_SNAPSHOT_KIVO_AUDIO_SNAPSHOT_H

#include <stddef.h>
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

typedef enum kivo_audio_decode_failure_category {
    KIVO_AUDIO_DECODE_FAILURE_SOURCE = 0,
    KIVO_AUDIO_DECODE_FAILURE_CONTAINER = 1,
    KIVO_AUDIO_DECODE_FAILURE_CODEC = 2,
    KIVO_AUDIO_DECODE_FAILURE_MEDIA_DATA = 3,
    KIVO_AUDIO_DECODE_FAILURE_CONVERSION = 4,
    KIVO_AUDIO_DECODE_FAILURE_PROCESSING = 5,
    KIVO_AUDIO_DECODE_FAILURE_GENERATION = 6,
    KIVO_AUDIO_DECODE_FAILURE_BOUNDARY = 7,
    KIVO_AUDIO_DECODE_FAILURE_CATEGORY_COUNT = 8
} kivo_audio_decode_failure_category;

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
    uint64_t render_underrun_events;
    uint64_t render_protocol_overrun_events;
    uint64_t queue_full_rejections;
    uint64_t queue_oversized_rejections;
    uint64_t producer_backpressure_events;
    uint64_t device_invalidation_events;
    uint64_t device_reopen_attempts;
    uint64_t device_reopen_successes;
    uint64_t device_reopen_failures;
    uint64_t format_negotiation_attempts;
    uint64_t format_negotiation_successes;
    uint64_t format_negotiation_failures;
    uint64_t format_renegotiation_attempts;
    uint64_t format_renegotiation_successes;
    uint64_t format_renegotiation_failures;
    uint64_t decode_failures_by_category[
        KIVO_AUDIO_DECODE_FAILURE_CATEGORY_COUNT];
    uint64_t recovery_attempts;
    uint64_t recovery_successes;
    uint64_t recovery_failures;
    uint64_t recovery_fallback_stops;
    uint64_t drain_attempts;
    uint64_t successful_drains;
    uint64_t failed_drains;
    uint64_t drain_timeouts;
    uint64_t stale_commands_rejected;
    uint64_t source_replacements;
    /* ABI 1.1.0 additive tail — playback timebase (gated by the capability flag
       KIVO_AUDIO_CAPABILITY_PLAYBACK_TIMEBASE_SNAPSHOT). All 0 == unknown on an
       older core. Field order keeps natural alignment (two uint32 before the
       uint64). render_sample_rate is the PROCESSED/negotiated render rate (NOT a
       bit-perfect claim); source_sample_rate + resample_active expose the
       resample fact. struct_version stays _1 (frozen prefix unchanged). */
    uint32_t render_sample_rate;
    uint32_t total_frames_known;
    uint64_t total_frames;
    uint32_t source_sample_rate;
    uint32_t resample_active;
} kivo_audio_snapshot_v1;

#define KIVO_AUDIO_SNAPSHOT_V1_BASE_SIZE \
    ((uint32_t)offsetof(kivo_audio_snapshot_v1, render_underrun_events))

#endif
