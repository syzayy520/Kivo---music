# P0-C-027 Format Descriptor Default Initializer Brace Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-027-FORMAT-DESCRIPTOR-DEFAULT-INITIALIZER-BRACE-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C format descriptor root value struct so default member initializers use brace form consistently with the current value-contract style.

The format descriptor family is already value-only. This task only rewrites field initializers in `AudioFormatDescriptor` from assignment form to brace form. It must not change fields, values, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or existing convenience methods.

## Scope

Update only this existing production header:

```text
include/kivo/core/contract/format/descriptor/audio_format_descriptor.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No enum value changes.
- No field name changes.
- No field default changes.
- No include path changes.
- No namespace changes.
- No operator changes.
- No convenience method changes.
- No runtime logic.
- No decoder, resampler, format negotiator, device format probe, platform API, source reader, thread, or output path logic.

## Natural family tree

Existing tree is retained exactly:

```text
format/
  descriptor/
    audio_format_descriptor.hpp
    sample_format.hpp
    channel_layout.hpp
    frame_layout.hpp
    channel_mask.hpp
  roles/
    native_decoded_format.hpp
    core_canonical_format.hpp
    render_format.hpp
    device_format.hpp
  negotiation/
    format_negotiation_result.hpp
    conversion_policy.hpp
    resample_decision.hpp
    bit_perfect_eligibility.hpp
    negotiated_output_mode.hpp
```

## Required transformation

For `AudioFormatDescriptor`, replace assignment-form default member initializers:

```cpp
SampleFormat sample_format = SampleFormat::Unknown;
ChannelLayout channel_layout = ChannelLayout::Unknown;
uint32_t sample_rate = 0;
uint8_t bits_per_sample = 0;
```

with brace-form equivalents:

```cpp
SampleFormat sample_format{SampleFormat::Unknown};
ChannelLayout channel_layout{ChannelLayout::Unknown};
uint32_t sample_rate{0};
uint8_t bits_per_sample{0};
```

## Invariants

- Format type count remains unchanged.
- File count remains 1 production header touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- Existing convenience methods remain unchanged.
- Existing operator declaration remains unchanged.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
assignment-form default initializers removed from AudioFormatDescriptor
brace-form default initializers present for all 4 fields
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + audio_format_descriptor.hpp
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-027-FORMAT-DESCRIPTOR-DEFAULT-INITIALIZER-BRACE-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
