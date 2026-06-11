# P1-06 Source / Decode Integration Boundary — Family Design Taskbook

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

Design the boundary between source resolution, container probing, packet flow, decode session, and decoded audio blocks.

## Candidate family areas

```text
source_decode/
  source_ref/
  probe_plan/
  packet_plan/
  decode_plan/
  format_plan/
  gapless_plan/
  failure_surface/
  status_snapshot/
```

## Must define in planning

```text
which P0 source and cue contracts are consumed
which P0-G seam docs are consumed
where format conversion is represented
where gapless metadata is carried
where source failure is represented
how remote source remains unsupported or opaque
```

## Non-goals

No real library adapter. No network adapter. No external decode API. No media database. No cover art or lyrics.

## Final planning classification

```text
KIVO-AUDIO-CORE-P1-06-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
