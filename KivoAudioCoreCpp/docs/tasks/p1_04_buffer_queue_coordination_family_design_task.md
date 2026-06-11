# P1-04 Buffer / Queue Coordination — Family Design Taskbook

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

Design product coordination between buffer policy, queue ownership, generation discard, and backpressure decisions.

## Candidate family areas

```text
buffer_queue/
  ownership/
  pressure/
  generation_guard/
  push_policy/
  pop_policy/
  discard_policy/
  snapshot/
```

## Must define in planning

```text
which P0 buffer and queue contracts are consumed
queue ownership direction
stale generation discard behavior
pressure reporting
underrun and overrun visibility
test family layout
```

## Non-goals

No output device. No decode. No lock-free implementation. No thread. No platform callback.

## Final planning classification

```text
KIVO-AUDIO-CORE-P1-04-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
