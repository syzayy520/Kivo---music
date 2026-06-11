# P0-C-035 Format Negotiation Constexpr Convenience Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-035-FORMAT-NEGOTIATION-CONSTEXPR-CONVENIENCE-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C format negotiation value structs so their pure convenience inspectors are explicitly `constexpr`, now that descriptor equality and descriptor inspectors are constexpr.

The format negotiation family is already value-only. This task only annotates existing side-effect-free convenience methods. It must not change fields, values, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or equality declarations.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/format/negotiation/
```

Allowed production headers:

```text
include/kivo/core/contract/format/negotiation/format_negotiation_result.hpp
include/kivo/core/contract/format/negotiation/conversion_policy.hpp
include/kivo/core/contract/format/negotiation/resample_decision.hpp
include/kivo/core/contract/format/negotiation/bit_perfect_eligibility.hpp
include/kivo/core/contract/format/negotiation/negotiated_output_mode.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No enum value changes.
- No field name changes.
- No field type changes.
- No field default changes.
- No include path changes.
- No namespace changes.
- No equality operator changes.
- No runtime logic.
- No decoder, resampler, format negotiator, device format probe, output mode executor, platform API, source reader, output writer, render thread, or output path logic.

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

For the selected format negotiation convenience inspectors, replace:

```cpp
[[nodiscard]] bool method_name(...) const noexcept
```

with:

```cpp
[[nodiscard]] constexpr bool method_name(...) const noexcept
```

Selected methods:

```text
FormatNegotiationResult::formats_match
ConversionPolicy::any_conversion_needed
ConversionPolicy::is_passthrough
ResampleDecision::is_needed
BitPerfectEligibility::is_eligible
NegotiatedOutputMode::is_bit_perfect
NegotiatedOutputMode::is_valid
```

## Invariants

- Format type count remains unchanged.
- File count remains 5 production headers touched.
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
all selected format negotiation convenience inspectors are explicitly constexpr
all selected format negotiation equality declarations remain constexpr and unchanged
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 5 format negotiation headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-035-FORMAT-NEGOTIATION-CONSTEXPR-CONVENIENCE-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
