# P0-C-028 Format Descriptor Constexpr Equality Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-028-FORMAT-DESCRIPTOR-CONSTEXPR-EQUALITY-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C format descriptor root value struct so its equality operator is explicitly `constexpr`, matching the pure value-contract style now used by neighboring state, policy, observability, error, source, command, CUE, identity, and generation families.

The format descriptor family is already value-only. This task only annotates the existing member `operator==` declaration. It must not change fields, values, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or existing convenience methods.

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

For `AudioFormatDescriptor`, replace:

```cpp
[[nodiscard]] bool operator==(const AudioFormatDescriptor&) const noexcept = default;
```

with:

```cpp
[[nodiscard]] constexpr bool operator==(const AudioFormatDescriptor&) const noexcept = default;
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
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
AudioFormatDescriptor contains [[nodiscard]] constexpr member operator==
non-constexpr member operator== removed from AudioFormatDescriptor
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + audio_format_descriptor.hpp
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-028-FORMAT-DESCRIPTOR-CONSTEXPR-EQUALITY-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
