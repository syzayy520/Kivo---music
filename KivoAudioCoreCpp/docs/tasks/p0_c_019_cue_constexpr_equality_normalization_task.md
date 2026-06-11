# P0-C-019 CUE Constexpr Equality Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-019-CUE-CONSTEXPR-EQUALITY-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C CUE value structs so equality operators are explicitly `constexpr`, matching the pure value-token style already used by several identity and generation contract families.

The CUE family is already value-only. This task only annotates existing member `operator==` declarations. It must not change fields, values, include boundaries, file tree, tests, CMake registration, or genealogy allowlists.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/cue/
```

Allowed production headers:

```text
include/kivo/core/contract/cue/boundary/cue_sheet_boundary.hpp
include/kivo/core/contract/cue/boundary/whole_album_file.hpp
include/kivo/core/contract/cue/boundary/virtual_track_segment.hpp
include/kivo/core/contract/cue/range/source_time_range.hpp
include/kivo/core/contract/cue/range/timeline_range.hpp
include/kivo/core/contract/cue/range/track_segment_range.hpp
include/kivo/core/contract/cue/continuity/codec_delay.hpp
include/kivo/core/contract/cue/continuity/padding.hpp
include/kivo/core/contract/cue/continuity/preroll.hpp
include/kivo/core/contract/cue/continuity/prefetch.hpp
include/kivo/core/contract/cue/continuity/gapless_metadata.hpp
include/kivo/core/contract/cue/continuity/cross_track_clock_continuity.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No field name changes.
- No field default changes.
- No include path changes.
- No namespace changes.
- No runtime logic.
- No parser, cue sheet loader, media scanner, decoder, clock executor, source reader, or track splitter logic.

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

For all 12 CUE structs, replace:

```cpp
[[nodiscard]] bool operator==(const Type&) const noexcept = default;
```

with:

```cpp
[[nodiscard]] constexpr bool operator==(const Type&) const noexcept = default;
```

## Invariants

- CUE type count remains unchanged.
- File count remains 12 production headers touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
all 12 CUE structs contain [[nodiscard]] constexpr member operator==
non-constexpr member operator== removed from those 12 structs
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 12 CUE headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-019-CUE-CONSTEXPR-EQUALITY-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
