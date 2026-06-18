#ifndef KIVO_HOST_ABI_SNAPSHOT_KIVO_AUDIO_TRUTH_H
#define KIVO_HOST_ABI_SNAPSHOT_KIVO_AUDIO_TRUTH_H

/* =============================================================================
 * Kivo Audio ABI 1.2.0 — bit-perfect truth (dedicated low-frequency struct).
 *
 * Truth changes only at open, not per playback frame, so it is exposed via a
 * dedicated kivo_audio_get_truth() getter rather than bloating the hot per-frame
 * snapshot. Gated by KIVO_AUDIO_CAPABILITY_BIT_PERFECT_TRUTH; absent => the host
 * must treat every field as Unknown.
 *
 * HONESTY: all fields encode "unknown" as a first-class state (mask bit clear /
 * 0xFFFFFFFF mode / zero sample_rate) so a zero-filled or older struct can never
 * be misread as a favourable value. C99 / fixed-width only; no C++/optional.
 * =============================================================================
 */

#include <stdint.h>

#include "kivo/host/abi/version/kivo_audio_abi_version.h"

/* Overall verdict. The host shows "Bit-Perfect" ONLY on _BIT_PERFECT. */
typedef enum kivo_audio_bit_perfect_verdict {
    KIVO_AUDIO_BIT_PERFECT_VERDICT_UNKNOWN = 0,
    KIVO_AUDIO_BIT_PERFECT_VERDICT_BIT_PERFECT = 1,
    KIVO_AUDIO_BIT_PERFECT_VERDICT_REJECTED = 2,
    KIVO_AUDIO_BIT_PERFECT_VERDICT_EVIDENCE_INCOMPLETE = 3
} kivo_audio_bit_perfect_verdict;

/* Mirrors core kivo::core::contract::BitPerfectRejectionCategory numeric values.
 * THIS ORDER IS ABI-FROZEN — a matching static_assert in the flattener pins it
 * to the C++ enum so a reorder cannot silently drift the wire value. */
typedef enum kivo_audio_bit_perfect_rejection_category {
    KIVO_AUDIO_BP_REJECT_UNKNOWN = 0,
    KIVO_AUDIO_BP_REJECT_FORMAT_MISMATCH = 1,
    KIVO_AUDIO_BP_REJECT_SAMPLE_RATE_MISMATCH = 2,
    KIVO_AUDIO_BP_REJECT_CHANNEL_LAYOUT_MISMATCH = 3,
    KIVO_AUDIO_BP_REJECT_PROCESSING_ACTIVE = 4,
    KIVO_AUDIO_BP_REJECT_ENGINE_PARTICIPATION = 5,
    KIVO_AUDIO_BP_REJECT_POLICY_FORBIDDEN = 6,
    KIVO_AUDIO_BP_REJECT_EVIDENCE_INCOMPLETE = 7,
    KIVO_AUDIO_BP_REJECT_SHARED_MODE = 8,
    KIVO_AUDIO_BP_REJECT_MODE_MISMATCH = 9,
    KIVO_AUDIO_BP_REJECT_RESAMPLING_ACTIVE = 10,
    KIVO_AUDIO_BP_REJECT_CONVERSION_ACTIVE = 11,
    KIVO_AUDIO_BP_REJECT_REPLAY_GAIN_ACTIVE = 12,
    KIVO_AUDIO_BP_REJECT_VOLUME_ACTIVE = 13,
    KIVO_AUDIO_BP_REJECT_DSP_ACTIVE = 14,
    KIVO_AUDIO_BP_REJECT_DITHER_ACTIVE = 15
} kivo_audio_bit_perfect_rejection_category;

/* Bit indices shared by known_mask (was-the-value-known) and bool_mask (the
 * value). PINNED — a matching static_assert lives in the flattener. */
typedef enum kivo_audio_truth_bool_bit {
    KIVO_AUDIO_TRUTH_BOOL_HOST_ENGINE_IN_PATH = 0,
    KIVO_AUDIO_TRUTH_BOOL_RESAMPLER_ACTIVE = 1,
    KIVO_AUDIO_TRUTH_BOOL_SAMPLE_FORMAT_CONVERSION = 2,
    KIVO_AUDIO_TRUTH_BOOL_BIT_DEPTH_CONVERSION = 3,
    KIVO_AUDIO_TRUTH_BOOL_CHANNEL_REMIX = 4,
    KIVO_AUDIO_TRUTH_BOOL_REPLAY_GAIN_ACTIVE = 5,
    KIVO_AUDIO_TRUTH_BOOL_VOLUME_ACTIVE = 6,
    KIVO_AUDIO_TRUTH_BOOL_DSP_ACTIVE = 7,
    KIVO_AUDIO_TRUTH_BOOL_DITHER_ACTIVE = 8,
    KIVO_AUDIO_TRUTH_BOOL_POLICY_ALLOWS_BIT_PERFECT = 9
} kivo_audio_truth_bool_bit;

/* Output mode sentinels for requested_mode / actual_mode. */
#define KIVO_AUDIO_TRUTH_MODE_SHARED 0u
#define KIVO_AUDIO_TRUTH_MODE_EXCLUSIVE 1u
#define KIVO_AUDIO_TRUTH_MODE_UNKNOWN 0xFFFFFFFFu

/* Flat PCM format descriptor (16 bytes, 4-byte aligned). sample_rate == 0 means
 * the format is unknown/absent. sample_format and frame_layout carry the core
 * enum's numeric value; bits_per_sample / container_bits are explicit so the
 * host need not decode the enum. */
typedef struct kivo_audio_format_descriptor {
    uint32_t sample_rate;
    uint32_t channel_mask;
    uint16_t channel_count;
    uint8_t sample_format;
    uint8_t bits_per_sample;
    uint8_t container_bits;
    uint8_t frame_layout;
    uint16_t reserved;
} kivo_audio_format_descriptor;

typedef struct kivo_audio_truth_v1 {
    uint32_t struct_size;
    uint32_t struct_version;
    uint64_t known_mask;   /* bit i (kivo_audio_truth_bool_bit) set => bool i known */
    uint64_t bool_mask;    /* bit i = value of bool i (meaningful iff known_mask bit set) */
    uint32_t verdict;             /* kivo_audio_bit_perfect_verdict */
    uint32_t rejection_category;  /* kivo_audio_bit_perfect_rejection_category */
    uint32_t evidence_complete;   /* 0 / 1 */
    uint32_t requested_mode;      /* KIVO_AUDIO_TRUTH_MODE_* */
    uint32_t actual_mode;         /* KIVO_AUDIO_TRUTH_MODE_* */
    uint32_t reserved;
    kivo_audio_format_descriptor source_format;
    kivo_audio_format_descriptor render_format;
    kivo_audio_format_descriptor device_format;
} kivo_audio_truth_v1;

/* The whole v1 struct is the frozen prefix; future fields append to the tail. */
#define KIVO_AUDIO_TRUTH_V1_BASE_SIZE ((uint32_t)sizeof(kivo_audio_truth_v1))

#endif
