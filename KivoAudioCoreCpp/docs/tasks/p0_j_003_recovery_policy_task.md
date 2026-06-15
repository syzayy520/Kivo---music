# P0-J-003 Recovery Policy and Session State

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-003-RECOVERY-POLICY
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Last Updated:** 2026-06-12

## Purpose

Classify recoverable playback failures and provide generation-scoped recovery
state transitions before wiring concrete device/source rebuild executors.

## Policy

- Device loss: reopen device, then rebuild output.
- Format change: reconfigure format, then reopen device.
- Source unavailable: rebuild source, then stop media.
- Codec failure/starvation: rebuild codec, then skip track.
- Buffer underrun/overrun: flush, then rebuild output.
- Unsupported format and permission failure are not silently retried.
- Internal bugs stop media and require reporting.

## Session Rules

- Only the active session may enter or complete recovery.
- Recovery preserves the prior Playing, Paused, or Ready state on success.
- Recovery that interrupts Seeking cancels the stale seek intent and restores
  Ready after success.
- Failed recovery enters Failed.
- Attempts, successes, failures, domain, and current action are observable.
- Closed and idle sessions cannot start recovery.
- Wrong-generation recovery requests are explicitly non-recoverable and do
  not change session state.

## Exit Criteria

- Recovery classification is constexpr-testable.
- Device and source recovery paths are covered.
- Old-generation completion is rejected.
- Success restores Playing and failure enters Failed.
- Recovery-interrupted seek and new-session error reset are unit-tested.
