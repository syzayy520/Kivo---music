# P0-F Platform Output Adapter Shell — Family Design Taskbook

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

P0-F reserves a platform output adapter shell without pulling platform details into core contracts. It must isolate platform-specific concepts behind adapter boundaries.

## Required family-design areas

```text
shared-mode path reservation
exclusive-mode path reservation
device capability query boundary
format support result boundary
format mismatch mapping
adapter lifecycle
device identity boundary
device generation visibility
reopen policy
buffer alignment policy
platform quarantine proof
```

## Candidate roots

```text
include/kivo/platform_output_adapter/
tests/platform_output_adapter/
```

The next assistant must verify actual repository conventions before choosing final paths.

## Non-goals

No full real playback. No direct core pollution. No platform type in core contracts. No hidden success path.

## Required planning draft

The next assistant must output Base Gate, Inventory Gate, ODR Gate, natural family tree, adapter shell boundary, tests, gates, STOP conditions, and implementation readiness verdict.

Final planning classification:

```text
KIVO-AUDIO-CORE-P0-F-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
