# P0-O Host ABI Integration Boundary Taskbook

**Status:** IMPLEMENTED AND VALIDATED
**Phase:** P0-O
**Date:** 2026-06-12

## Objective

Expose the proven audio engine through a stable product boundary without
allowing application framework, source credentials, platform SDK types, or
internal queue ownership to enter the core.

## Base Gate

- P0-N processing and output truth are complete.
- FFmpeg decode and WASAPI render targets are available.
- Playback runtime owns state, queue, generation, seek, drain, and recovery.
- The host boundary depends inward; no core target depends on the host ABI.

## Inventory Gate

The ABI must cover:

- lifecycle and opaque handle ownership;
- capability and ABI version negotiation;
- host-resolved byte sources;
- open, transport, seek, close, and shutdown commands;
- bounded pump execution;
- sanitized state and diagnostic snapshots;
- source callback ownership and thread rules.

The ABI must not expose:

- C++ classes, STL, exceptions, COM, Windows SDK, or FFmpeg types;
- render or decode queue references;
- file paths, URLs, credentials, access tokens, or host library records;
- frontend state, Qt commands, QML state, or product adapter ownership.

## ODR Gate

One implementation owner exists for each concern:

- `api`: exported C entry points and exception containment;
- `engine`: product runtime composition and operation families;
- `handle`: token registry and stale-handle rejection;
- `source`: host callback to byte-source adaptation;
- `mapping`: core-to-host result and state mapping;
- `validation`: versioned structure validation.

No second playback state machine, queue, decoder, renderer, or source resolver
is introduced by P0-O.

## Natural Family Tree

```text
include/kivo/host/abi/
  api/
  capability/
  command/
  configuration/
  diagnostic/
  handle/
  result/
  snapshot/
  source/
  version/

src/host/abi/
  api/
  engine/
  handle/
  mapping/
  source/
  validation/

tests/host_abi/
  compatibility/
  fixture/
  runner/
  scenario/

tests/hardware/host_abi/
  fixture/
  runner/
  scenario/
```

Each file remains below the repository's 260-line responsibility gate.

## Host Boundary

- Handles are 64-bit opaque tokens resolved through a synchronized registry.
- Every ABI structure begins with `struct_size` and `struct_version`.
- Version 1 accepts larger structures and ignores unknown tail fields.
- Source data is provided through read, seek, and release callbacks.
- Valid accepted sources transfer ownership exactly once to the core.
- Commands use strictly increasing IDs and explicit session generations.
- Concurrent command entry returns `KIVO_AUDIO_RESULT_BUSY`.
- Pumping is bounded by a caller-provided step budget.
- Snapshots are copies of sanitized facts, never queue control surfaces.
- Diagnostics contain numeric domains and codes only.

## Qt Product Adapter Ownership

The future Qt/C++ product layer owns:

- URI/path resolution, NAS/WebDAV/cloud authentication, and credential refresh;
- native worker scheduling for serialized ABI calls and pumping;
- conversion from ABI snapshots to Qt models, signals, and frontend events;
- application shutdown ordering and DLL packaging.

The audio core owns decode, render, playback state, bounded queues, generation
checks, processing, and device recovery. Qt is an adapter consumer and never
becomes the audio engine owner.

## Tests

- Every public C header compiles independently as C11.
- The umbrella API compiles and links from C and C++.
- Larger version-1 structures are accepted.
- Random and stale handles return `INVALID_HANDLE`.
- Shutdown is terminal while destroy remains valid.
- Accepted source ownership releases exactly once on rejection.
- Hardware smoke proves host callback source to FFmpeg to WASAPI completion.

## Stop Conditions

P0-O fails if any ABI type contains a C++ or platform type, a source can be
released twice, invalid handles can be dereferenced, commands bypass runtime
serialization, diagnostics expose source secrets, or the host can manipulate
internal queues.

## Verdict

```text
KIVO-AUDIO-CORE-P0-O-HOST-ABI-001
IMPLEMENTATION_COMPLETE
P0-P_RELEASE_HARDENING_READY
```
