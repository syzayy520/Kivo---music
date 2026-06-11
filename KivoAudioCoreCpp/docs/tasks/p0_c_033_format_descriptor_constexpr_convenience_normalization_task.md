# P0-C-033 Format Descriptor Constexpr Convenience Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-033-FORMAT-DESCRIPTOR-CONSTEXPR-CONVENIENCE-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C format descriptor root value struct so its pure convenience inspectors are explicitly `constexpr`, matching the value-contract style already used by `channel_layout.hpp` metadata helpers and by the descriptor equality operator.

The format descriptor family is already value-only. This task only annotates existing side-effect-free convenience methods. It must not change fields, values, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or equality declarations.

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
- No equality operator changes.
- No runtime logic.
- No decoder, resampler, format negotiator, device format probe, platform API, source reader, output writer, render thread, or output path logic.

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
[[nodiscard]] uint8_t channel_count() const noexcept
[[nodiscard]] uint8_t bytes_per_frame() const noexcept
[[nodiscard]] bool is_valid() const noexcept
```

with:

```cpp
[[nodiscard]] constexpr uint8_t channel_count() const noexcept
[[nodiscard]] constexpr uint8_t bytes_per_frame() const noexcept
[[nodiscard]] constexpr bool is_valid() const noexcept
```

## Invariants

- Format type count remains unchanged.
- File count remains 1 production header touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- Existing equality operator remains unchanged.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
AudioFormatDescriptor convenience inspectors are explicitly constexpr
AudioFormatDescriptor equality declaration remains unchanged
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + audio_format_descriptor.hpp
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-033-FORMAT-DESCRIPTOR-CONSTEXPR-CONVENIENCE-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
