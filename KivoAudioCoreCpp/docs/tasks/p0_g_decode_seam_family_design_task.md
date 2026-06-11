# P0-G Decode Seam — Family Design Taskbook

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

P0-G designs a layered decode seam. It must not collapse source, container, packet, session, format, and seek concepts into one large decoder file.

## Required family-design areas

```text
source byte provider seam
container boundary seam
packet source seam
decode session seam
audio block contract
seekable source contract
media probe result
gapless metadata bridge
codec delay and padding bridge
decode generation
format conversion request
```

## Candidate roots

```text
include/kivo/decode_seam/
tests/decode_seam/
```

The next assistant must verify actual repository conventions before choosing final paths.

## Non-goals

No external library leak into core. No network source implementation. No NAS adapter. No all-in-one decoder manager.

## Required planning draft

The next assistant must output Base Gate, Inventory Gate, ODR Gate, natural family tree, seam boundary, tests, gates, STOP conditions, and implementation readiness verdict.

Final planning classification:

```text
KIVO-AUDIO-CORE-P0-G-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
