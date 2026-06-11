# P1 Audio Core Product Roadmap — Taskbook

Planning only. No implementation.

Owner approval required before any implementation task:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Purpose

P1 moves from P0 contracts and boundaries into product-grade playback-core construction. Each P1 task must still begin with a planning draft and a natural family tree before code is created.

## Required sequence

```text
P1-01 core service skeleton
P1-02 session lifecycle
P1-03 scheduler and clock coordination
P1-04 buffer and queue coordination
P1-05 device management boundary
P1-06 source and decode integration boundary
P1-07 quality gates and CI hardening
P1-08 host adapter integration boundary
```

## Global rules

```text
one task at a time
planning draft first
owner review second
implementation only after approval
folder equals family
subfolder equals subfamily
single file equals minimum responsibility
no all-in-one manager
no bucket utilities
no UI coupling inside audio core
```

## Baseline expectation

P0-C through later P0 taskbooks must be read before P1 begins. P1 must not skip any unfinished P0 safety gate unless Owner explicitly pauses that dependency.
