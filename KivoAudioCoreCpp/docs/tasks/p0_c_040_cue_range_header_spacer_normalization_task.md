# P0-C-040 CUE Range Header Spacer Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-040-CUE-RANGE-HEADER-SPACER-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C CUE range headers so the file banner is followed by a clean single spacer before `#pragma once`, matching the contract header layout already used by the surrounding value-contract families.

This is a formatting-only contract readability cleanup. It must not change fields, values, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or equality declarations.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/cue/range/
```

Allowed production headers:

```text
include/kivo/core/contract/cue/range/source_time_range.hpp
include/kivo/core/contract/cue/range/timeline_range.hpp
include/kivo/core/contract/cue/range/track_segment_range.hpp
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
- No equality operator signature changes.
- No runtime logic.
- No CUE parser, segment mapper, source reader, timeline engine, output path, or playback logic.

## Natural family tree

Existing tree is retained exactly:

```text
cue/
  boundary/
    cue_sheet_boundary.hpp
    whole_album_file.hpp
    virtual_track_segment.hpp
  range/
    source_time_range.hpp
    timeline_range.hpp
    track_segment_range.hpp
  continuity/
    codec_delay.hpp
    padding.hpp
    preroll.hpp
    prefetch.hpp
    gapless_metadata.hpp
    cross_track_clock_continuity.hpp
```

## Required transformation

Remove duplicated empty banner spacer lines between the banner comment and `#pragma once` in the selected CUE range headers.

Keep the semantic banner and all declarations unchanged.

## Invariants

- CUE type count remains unchanged.
- File count remains 3 production headers touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field types change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- Existing equality operators remain signature-identical.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
selected CUE range headers have clean banner-to-pragma spacing
selected equality operator declarations remain constexpr and signature-identical
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 3 CUE range headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-040-CUE-RANGE-HEADER-SPACER-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
