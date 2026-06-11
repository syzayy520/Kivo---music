# P0-O Host ABI / Integration Boundary — Family Design Taskbook

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

P0-O plans the host integration boundary while keeping AudioCore independent from app framework details.

## Required family-design areas

```text
host source resolver boundary
opaque source handle
playback plan
transition plan
queue snapshot
host command serialization
host adapter ownership
host-visible result mapping
integration error surface
```

## Candidate roots

```text
include/kivo/host_boundary/
tests/host_boundary/
```

The next assistant must verify actual repository conventions before choosing final paths.

## Non-goals

No frontend implementation. No app framework binding. No UI store. No direct media library indexing. No credential handling in core.

## Required planning draft

The next assistant must output Base Gate, Inventory Gate, ODR Gate, natural family tree, host boundary, tests, gates, STOP conditions, and implementation readiness verdict.

Final planning classification:

```text
KIVO-AUDIO-CORE-P0-O-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
