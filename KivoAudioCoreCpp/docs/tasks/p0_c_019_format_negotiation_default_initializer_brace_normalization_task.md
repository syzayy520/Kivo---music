# P0-C-019 Format Negotiation Default Initializer Brace Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-019-FORMAT-NEGOTIATION-DEFAULT-INITIALIZER-BRACE-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C-004 format negotiation value structs so default member initializers use brace form consistently with the current value-contract style.

The format negotiation family is already value-only. This task only rewrites field initializers from assignment form to brace form. It must not change fields, values, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or existing convenience methods.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/format/negotiation/
```

Allowed production headers:

```text
include/kivo/core/contract/format/negotiation/format_negotiation_result.hpp
include/kivo/core/contract/format/negotiation/conversion_policy.hpp
include/kivo/core/contract/format/negotiation/bit_perfect_eligibility.hpp
include/kivo/core/contract/format/negotiation/negotiated_output_mode.hpp
include/kivo/core/contract/format/negotiation/resample_decision.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No enum value changes.
- No field name changes.
- No operator changes.
- No convenience method changes.
- No comment removal.
- No runtime logic.
- No platform, source, device, command, state, resampler implementation, output device policy, or thread logic.

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

Replace assignment-form default member initializers:

```cpp
bool resample_needed = false;
OutputMode mode = OutputMode::Unknown;
uint32_t source_rate = 0;
```

with brace-form equivalents:

```cpp
bool resample_needed{false};
OutputMode mode{OutputMode::Unknown};
uint32_t source_rate{0};
```

Apply only to fields in the five allowed negotiation structs.

## Invariants

- Format type count remains unchanged.
- File count remains 5 production headers touched.
- Cross-family direct includes remain 0 beyond existing format-family internal includes.
- No field names change.
- No field defaults change.
- No namespace changes.
- Operator declarations remain unchanged.
- Existing convenience methods remain unchanged.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
assignment-form default initializers removed from the five allowed format negotiation structs
brace-form default initializers present for all touched fields
cross-family direct includes = 0 beyond existing format-family internal includes
no new runtime forbidden tokens introduced
changed files limited to this taskbook + 5 format negotiation headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-019-FORMAT-NEGOTIATION-DEFAULT-INITIALIZER-BRACE-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
