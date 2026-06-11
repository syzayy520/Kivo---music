# P0-C-041 CUE Boundary Header Blank Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-041-CUE-BOUNDARY-HEADER-BLANK-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C CUE boundary headers so the file banner is followed by a single blank separator before `#pragma once`, matching the cleaner value-contract header layout used by neighboring CUE range and continuity headers.

This is a formatting-only contract readability cleanup. It must not change fields, values, include boundaries, file tree, tests, CMake registration, genealogy allowlists, or equality declarations.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/cue/boundary/
```

Allowed production headers:

```text
include/kivo/core/contract/cue/boundary/cue_sheet_boundary.hpp
include/kivo/core/contract/cue/boundary/whole_album_file.hpp
include/kivo/core/contract/cue/boundary/virtual_track_segment.hpp
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
- No equality operator signature changes.
- No runtime logic.
- No CUE parser, decoder, timeline mapper, source reader, render thread, or output path logic.

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

For the three selected CUE boundary headers, normalize the banner tail so it becomes:

```cpp
// =============================================================================

#pragma once
```

Remove stray standalone `//` banner stubs and duplicate blank lines between the banner and `#pragma once`.

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
selected CUE boundary headers have a single blank separator before #pragma once
no stray standalone // remains between banner and #pragma once
selected equality operator declarations remain constexpr and signature-identical
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 3 CUE boundary headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-041-CUE-BOUNDARY-HEADER-BLANK-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
