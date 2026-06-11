# P0-C-034 Format Roles Constexpr Convenience Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-034-FORMAT-ROLES-CONSTEXPR-CONVENIENCE-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C format role wrapper value structs so their pure convenience inspectors are explicitly `constexpr`, now that `AudioFormatDescriptor::is_valid()` is also constexpr.

The format roles family is already value-only. This task only annotates existing side-effect-free convenience methods. It must not change fields, values, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or equality declarations.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/format/roles/
```

Allowed production headers:

```text
include/kivo/core/contract/format/roles/native_decoded_format.hpp
include/kivo/core/contract/format/roles/core_canonical_format.hpp
include/kivo/core/contract/format/roles/render_format.hpp
include/kivo/core/contract/format/roles/device_format.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No field name changes.
- No field type changes.
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

For the four selected format role structs, replace:

```cpp
[[nodiscard]] bool is_valid() const noexcept
```

with:

```cpp
[[nodiscard]] constexpr bool is_valid() const noexcept
```

## Invariants

- Format type count remains unchanged.
- File count remains 4 production headers touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field types change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- Existing equality operators remain unchanged.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
all 4 selected format role structs contain [[nodiscard]] constexpr bool is_valid() const noexcept
all 4 selected format role equality declarations remain constexpr and unchanged
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 4 format role headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-034-FORMAT-ROLES-CONSTEXPR-CONVENIENCE-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
