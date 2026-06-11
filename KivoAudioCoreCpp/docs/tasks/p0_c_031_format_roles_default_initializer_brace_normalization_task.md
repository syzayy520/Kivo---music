# P0-C-031 Format Roles Default Initializer Brace Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-031-FORMAT-ROLES-DEFAULT-INITIALIZER-BRACE-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C format role wrapper value structs so their `AudioFormatDescriptor` members use explicit brace-form default member initializers, matching the value-contract style now used by the format descriptor root and neighboring contract families.

The format roles family is already value-only. This task only adds explicit `{}` default member initializers to existing wrapper fields. It must not change field names, field types, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or existing convenience methods.

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
- No include path changes.
- No namespace changes.
- No operator changes.
- No convenience method changes.
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

For the four selected format role structs, replace implicit member defaulting:

```cpp
AudioFormatDescriptor format;
```

with explicit brace-form defaulting:

```cpp
AudioFormatDescriptor format{};
```

## Invariants

- Format type count remains unchanged.
- File count remains 4 production headers touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field types change.
- No include path changes.
- No namespace changes.
- Existing convenience methods remain unchanged.
- Existing operator declarations remain unchanged.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
all 4 selected format role structs contain AudioFormatDescriptor format{}
implicit AudioFormatDescriptor format; removed from those 4 structs
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 4 format role headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-031-FORMAT-ROLES-DEFAULT-INITIALIZER-BRACE-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
