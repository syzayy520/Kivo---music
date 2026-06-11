# P1-08 Host Adapter Integration Boundary — Family Design Taskbook

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

Design the host adapter boundary that lets a desktop app call the audio core without allowing app state or UI concepts to pollute core families.

## Candidate family areas

```text
host_adapter/
  request/
  response/
  playback_plan/
  source_handle/
  queue_snapshot/
  status_snapshot/
  boundary_error/
```

## Must define in planning

```text
what the host can request
what the core can return
how opaque source handles are represented
how queue snapshots cross the boundary
how errors are mapped without exposing internals
what remains outside AudioCore
```

## Non-goals

No frontend binding. No app framework code. No database code. No file scanner. No settings UI.

## Final planning classification

```text
KIVO-AUDIO-CORE-P1-08-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
