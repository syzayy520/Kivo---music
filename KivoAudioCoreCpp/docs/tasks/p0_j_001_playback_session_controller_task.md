# P0-J-001 Playback Session Controller

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-001-PLAYBACK-SESSION-CONTROLLER
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Last Updated:** 2026-06-12

## Purpose

Create the first executable product-control layer above decode and render. It
serializes command intake, owns session generation and lifecycle truth, freezes
pause position, rejects stale work, and guarantees terminal shutdown behavior.

## Natural Family Tree

```text
include/kivo/core/playback/session/
  playback_command.hpp
  playback_command_failure.hpp
  playback_command_result.hpp
  playback_session_snapshot.hpp
  playback_session_controller.hpp

src/core/playback/session/
  playback_session_controller.cpp

tests/playback_session/
  playback_session_tests.cpp
```

## Rules

- Control code may lock; realtime render code never calls this controller.
- Command IDs are globally monotonic.
- Source replacement requires a strictly newer session generation.
- Commands for an old session cannot mutate the current session.
- Pause freezes the last accepted rendered position.
- Position updates are monotonic and accepted only while playing.
- Close-source succeeds from failed state.
- Shutdown is terminal and idempotent.
- Unsupported commands return explicit rejection.

## Initial Scope

- Open/replace source lifecycle truth.
- Start/resume, pause, stop, close-source, and shutdown.
- Serialized multi-caller command intake.
- Position and failure reports scoped by session generation.
- Recovery execution, decoder ownership, and renderer ownership remain the next
  P0-J slices. Seek and flush command semantics continue in P0-J-002.

## Exit Criteria

- Deterministic lifecycle and pause tests pass.
- Old-session and out-of-order commands are rejected.
- Concurrent command submission leaves a coherent snapshot.
- Failed state can close and shutdown.
- Public headers remain platform and dependency neutral.
