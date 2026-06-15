# P0-O Host ABI Closeout

**Status:** IMPLEMENTATION COMPLETE
**Date:** 2026-06-12

## Delivered

- Versioned C ABI 1.0.0 and C11 standalone headers.
- Opaque 64-bit handle registry with stale-handle rejection.
- Extensible size/version structures and capability negotiation.
- Host-resolved read, seek, size, and release source callbacks.
- Explicit source ownership and callback-thread contract.
- Serialized open, transport, seek, close, shutdown, and bounded pump calls.
- Sanitized numeric diagnostics and host-visible snapshots.
- No C++, STL, COM, Windows SDK, FFmpeg, queue, path, or credential types in
  the ABI.
- Tauri integration contract that keeps framework and resolver ownership in
  the host.

## Family Gate

Public ABI types are grouped by API, capability, command, configuration,
diagnostic, handle, result, snapshot, source, and version. Implementation is
grouped by API, engine operation, handle, mapping, source, and validation.
Tests are grouped by compatibility, fixtures, runner, and behavior scenarios.

The largest P0-O implementation file is 209 lines.

## Verification

- MSVC C11/C++20 `/W4 /WX` ABI build: passed.
- Public C headers standalone compilation: passed.
- ABI compatibility and lifecycle tests: passed.
- Random handle, stale handle, shutdown, and release-once tests: passed.
- Full CTest suite: 16 of 16 passed.
- Repository governance gates: 9 of 9 passed.
- Host callback source to FFmpeg to WASAPI hardware smoke: 96,000 rendered
  frames and successful render completion.

## Next Phase

P0-P owns release hardening: reproducible artifacts, signing, provenance,
third-party notices, SBOM, symbols, crash privacy, upgrade/rollback behavior,
release matrices, and commercial claim review.
