# P1-01 Core Service Skeleton — Family Design Taskbook

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

Design the first product-core service skeleton that owns no real playback behavior yet. It should provide a clean home for future lifecycle, scheduling, queue, device, source, and output coordination families.

## Candidate family areas

```text
core_service/
  identity/
  configuration/
  lifecycle_port/
  dependency_port/
  status_snapshot/
  contract_bridge/
```

## Must define in planning

```text
root namespace
allowed folders
forbidden folders
public include boundary
internal include boundary
which P0 contracts are consumed
which runtime behavior is still forbidden
unit test shape
integration test shape
```

## Non-goals

No actual audio output. No decode. No source open. No device open. No thread loop. No UI binding.

## Final planning classification

```text
KIVO-AUDIO-CORE-P1-01-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
