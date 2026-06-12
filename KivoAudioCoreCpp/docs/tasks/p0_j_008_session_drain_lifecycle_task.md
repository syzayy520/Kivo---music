# P0-J-008 Session Drain Lifecycle

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-008-SESSION-DRAIN-LIFECYCLE
**Status:** COMPLETED
**Execution Basis:** P0-J EOS and drain completion
**Last Updated:** 2026-06-12

## Purpose

Give the serialized playback Session an explicit, generation-safe drain
lifecycle before the Runtime connects EOS to the render boundary.

## Natural Family Tree

```text
include/kivo/core/playback/session/
  drain/
    playback_drain_completion.hpp
    playback_drain_ticket.hpp

src/core/playback/session/
  drain/
    playback_session_drain.cpp

tests/playback_session/
  scenario/
    drain_lifecycle_tests.cpp
```

Existing controller, state, snapshot, command, runner, and build files may be
updated only to expose or register this family.

## Responsibility Rules

- `PlaybackDrainTicket` identifies one drain attempt by both Session and drain
  generation.
- `PlaybackDrainCompletion` describes only terminal drain outcome.
- The drain implementation owns begin/complete state transitions only.
- Command handling owns user-command races during drain.
- Snapshot fields own drain observability only.
- Scenario tests own drain state and stale-completion assertions only.

## State Rules

- Drain may begin only from `Playing` with a matching active Session
  generation.
- Begin transitions `Playing -> Draining` and returns a nonzero drain ticket.
- Successful completion transitions `Draining -> Stopped`.
- Failed completion transitions `Draining -> Failed`.
- Completion must match both Session and drain generation.
- Pause while draining is rejected.
- Stop while draining cancels the active drain and transitions to `Stopped`.
- Close while draining cancels the active drain and transitions to `Idle`.
- Shutdown while draining cancels the active drain and transitions to
  `Closed`.
- A completion arriving after stop, close, shutdown, or a later drain attempt
  is stale and cannot mutate state.

## Non-Goals

- No renderer `drain()` call.
- No timeout policy.
- No worker thread or asynchronous callback.
- No EOS detection changes.
- No generic helper, utility, detail, common, or miscellaneous directory.

## Gates

- The new public and implementation files live only under the declared
  `drain/` branches.
- Session roots remain free of directly flattened source files.
- Every Session production and test file remains below 260 lines.
- New public headers compile independently.
- MSVC `/W4 /WX`, all CTest executables, and all cumulative gates pass.

## Exit Criteria

- Drain begin and terminal completion are explicit and generation-safe.
- Close, shutdown, stop, and pause races have deterministic tests.
- Drain counters expose attempts, successes, failures, and cancellations.
- No stale completion can mutate a newer or terminated playback state.

## Completion Evidence

- Drain tickets bind Session and drain generations.
- Generation exhaustion rejects a new drain instead of reusing a ticket.
- Successful completion preserves the terminal rendered position and enters
  `Stopped`; failed completion enters `Failed`.
- Pause, stop, close, shutdown, and source replacement have deterministic
  drain behavior.
- Playback Session tests passed 17 of 17 scenarios.
- Maximum Session production file size is 216 lines.
- Maximum Session test file size is 117 lines.
- MSVC Debug build passed with `/W4 /WX`.
- CTest passed 11 of 11 test executables.
- All 9 cumulative audio-core gates passed.
