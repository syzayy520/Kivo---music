# P0-H Minimum Real Output — Family Design Taskbook

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

P0-H plans the first minimum real output path while preventing demo-ware. Every unsupported feature must be explicit and every core boundary must remain clean.

## Required family-design areas

```text
device open and close boundary
format negotiation visibility
start stop reset semantics
underrun metric boundary
device invalidated mapping
no core pollution proof
unsupported feature reporting
shared exclusive status visibility
device generation visibility
```

## Candidate roots

```text
include/kivo/minimum_output/
tests/minimum_output/
```

The next assistant must verify actual repository conventions before choosing final paths.

## Non-goals

No full player. No decode integration unless explicitly approved. No fake success. No product UI.

## Required planning draft

The next assistant must output Base Gate, Inventory Gate, ODR Gate, natural family tree, minimum output boundary, tests, gates, STOP conditions, and implementation readiness verdict.

Final planning classification:

```text
KIVO-AUDIO-CORE-P0-H-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
