# P0-I Decode To Output Loop — Family Design Taskbook

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

P0-I plans the first decode-to-output loop while keeping every unsupported behavior explicit. It must not silently truncate, silently convert, or hide failure states.

## Required family-design areas

```text
decode generation
source generation
format conversion policy
seek unsupported state
flush unsupported state
device format mismatch behavior
source failure visibility
remote source unsupported state
core boundary proof
```

## Candidate roots

```text
include/kivo/decode_output_loop/
tests/decode_output_loop/
```

The next assistant must verify actual repository conventions before choosing final paths.

## Non-goals

No full media player. No remote source adapter. No silent fallback. No hidden conversion. No UI.

## Required planning draft

The next assistant must output Base Gate, Inventory Gate, ODR Gate, natural family tree, decode-output loop boundary, tests, gates, STOP conditions, and implementation readiness verdict.

Final planning classification:

```text
KIVO-AUDIO-CORE-P0-I-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
