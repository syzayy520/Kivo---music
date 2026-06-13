# Host ABI Integration Contract

**ABI:** Kivo Audio ABI 1.0.0
**Applies to:** `kivo_audio_core.dll`
**Date:** 2026-06-12
**Last Updated:** P0-O, 2026-06-12

## Purpose

Define the stable integration, ownership, threading, privacy, and
compatibility rules between the Kivo host application and audio core.

## Scope

This policy applies to all public C headers, exported DLL functions, host
source callbacks, diagnostics, snapshots, and future Tauri adapter code.

## Current Phase

P0-O is complete. P0-P may package and sign this ABI but may not weaken its
ownership, privacy, compatibility, or queue-isolation rules.

## Allowed

- Versioned C structures and fixed-width scalar types.
- Opaque handles, host byte callbacks, serialized commands, bounded pumping,
  sanitized diagnostics, and copied snapshots.
- Host-side source resolution, credentials, scheduling, and UI translation.

## Forbidden

- C++, STL, exceptions, COM, Windows SDK, or FFmpeg types in public ABI.
- Direct render/decode queue access or a second playback state machine.
- Paths, URLs, credentials, access tokens, or raw platform strings in
  diagnostics.

## Boundary Rule

The C ABI is the only product integration boundary. Host applications may
submit commands, provide a byte source, pump bounded work, and read sanitized
snapshots. They may not access decoder, renderer, processing chain, or queue
objects.

The public entry header is:

```text
include/kivo/host/abi/api/kivo_audio_api.h
```

## Structure Versioning

Every public structure starts with:

```c
uint32_t struct_size;
uint32_t struct_version;
```

Callers zero-initialize a structure, set its size and version, then fill known
fields. ABI 1 accepts a structure larger than the known version-1 size and
ignores its tail. A major ABI change may break compatibility. Minor and patch
changes must preserve all existing version-1 layouts and enum values.

## Handle Lifetime

`kivo_audio_create` returns an opaque nonzero token. It is not a pointer.
Random values, zero, and tokens used after destroy return
`KIVO_AUDIO_RESULT_INVALID_HANDLE`.

`kivo_audio_destroy` is always required, including after shutdown. Shutdown is
terminal for engine commands but does not invalidate the token until destroy.

## Source Ownership

The host resolves local files, NAS, WebDAV, cloud objects, and credentials. It
passes only an opaque context and byte callbacks to the ABI.

Ownership rules:

1. Invalid handle or invalid structure: ownership remains with the host.
2. Once valid arguments are accepted for open: ownership transfers to core.
3. Core invokes `release` exactly once, including semantic open rejection.
4. Read and seek may be called before `kivo_audio_open_source` returns.
5. The host keeps callback code and context storage valid until `release`.
6. Credentials, URLs, and paths must remain inside the host resolver context.

Known size and seek support are declared through source flags. Callback output
larger than the supplied buffer is rejected as a source failure.

## Command Ordering

Open and transport commands are serialized by the host on one native control
worker. The ABI also uses a non-reentrant command gate and returns `BUSY` for
overlap.

- Command IDs are nonzero and strictly increasing per handle.
- Open session generations are nonzero and strictly increasing.
- Transport commands use the active session generation.
- Seek uses an absolute PCM frame position.
- Shutdown uses zero generation only when no session is active.

The ABI does not create a second state machine. Every command is translated to
the existing playback runtime.

## Pump Contract

`kivo_audio_pump` executes at most `maximum_steps` decode/render iterations.
The host calls it repeatedly from a native worker, not the JavaScript/UI
thread. A no-progress result should be retried after a short wait or a host
scheduler yield.

`reached_end_of_stream` means render completion, not merely decoder
exhaustion. The host must not stop pumping when the decoder first reaches end
of input.

Source callbacks execute synchronously on the thread calling open or pump.
Diagnostic callbacks execute synchronously on the calling control thread.
No host callback is invoked from a WASAPI realtime render callback.

## Diagnostics and Privacy

Diagnostics contain fixed numeric fields only:

- sequence;
- command ID;
- domain;
- stable code;
- ABI result.

They never contain paths, URLs, hostnames, credentials, tokens, media tags, or
raw platform error strings. The host may enrich diagnostics only after
applying its own privacy and redaction policy.

## Tauri Integration

The Tauri adapter is a consumer of the ABI. It should:

1. Load and own one ABI handle per playback engine instance.
2. Resolve application source IDs into callback contexts.
3. Run open, commands, pumping, and destroy on a native serialized worker.
4. Convert snapshots to bounded application events.
5. Keep Rust panics and JavaScript exceptions outside all C callbacks.
6. Destroy the handle before unloading the DLL or callback module.

The adapter must not mirror internal queues, issue FFmpeg calls, create a
second playback state machine, or pass secrets into diagnostic fields.

## Compatibility Gate

A release is ABI-compatible only when:

- all public C headers compile independently under C11;
- old version-1 callers compile and run unchanged;
- enum values and structure prefixes remain stable;
- larger version-1 structures remain accepted;
- invalid and stale handles remain non-dereferenceable;
- callback ownership tests and shutdown tests pass.

## Gate

The C11 standalone-header build, ABI tests, genealogy gate, forbidden-token
gate, hardware callback-source smoke, and full CTest suite enforce this
policy.

## Failure

Any public type leak, stale-handle dereference, double release, command-order
bypass, queue exposure, secret exposure, or incompatible version-1 layout is
a release blocker.

## Future Phase

P0-P owns packaging, signing, symbols, SBOM, crash privacy, and release
matrices. A future Tauri adapter consumes this ABI without taking core
ownership.

## Deferred

ABI 2, non-Windows product adapters, asynchronous event delivery, and
additional source capability versions require separate evidence and explicit
version policy review.
