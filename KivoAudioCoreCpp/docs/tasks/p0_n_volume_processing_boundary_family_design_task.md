# P0-N Volume / Processing Boundary — Family Design Taskbook

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

P0-N plans boundaries for gain, volume, processing, conversion, channel remix, and dither policy without polluting bit-perfect truth claims.

## Required family-design areas

```text
gain application point
volume application point
fixed versus float boundary
processing enabled state
conversion bypass path
conversion latency frames
tail handling
dither policy
channel remix policy
bit-perfect incompatibility rule
```

## Candidate roots

```text
include/kivo/processing_boundary/
tests/processing_boundary/
```

The next assistant must verify actual repository conventions before choosing final paths.

## Non-goals

No DSP implementation. No resampler implementation. No UI slider. No hidden bit-perfect override.

## Required planning draft

The next assistant must output Base Gate, Inventory Gate, ODR Gate, natural family tree, processing boundary, tests, gates, STOP conditions, and implementation readiness verdict.

Final planning classification:

```text
KIVO-AUDIO-CORE-P0-N-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
