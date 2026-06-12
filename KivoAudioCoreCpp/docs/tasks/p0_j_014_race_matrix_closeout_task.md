# P0-J-014 Race Matrix Closeout

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-014-RACE-MATRIX-CLOSEOUT
**Status:** COMPLETED
**Execution Basis:** P0-J required race matrix
**Last Updated:** 2026-06-12

## Purpose

Close the remaining explicit P0-J race gaps: pause during source replacement
and device loss during flush.

## Natural Family Tree

```text
tests/fake_renderer/
  scenario/
    device_lifecycle_fault_tests.cpp

tests/playback_runtime/coordinator/scenario/
  flush_recovery_tests.cpp
  track_switch_race_tests.cpp
```

## Responsibility Rules

- Fake renderer owns deterministic `NextFlush` device-loss injection.
- Runtime flush owns promotion of device loss into the existing recovery path.
- Track-switch tests own stale-command isolation across serialized replacement.
- No playlist policy is added to the audio core.

## Exit Criteria

- Device loss during flush enters Recovering and can reopen successfully.
- Pause from the replaced Session cannot mutate the new renderer.
- All required P0-J race scenarios have deterministic evidence.
- Full build, CTest, and cumulative gates pass.

## Completion Evidence

- Rapid replacement retains only the latest Session generation.
- Pause from the replaced Session is rejected after a blocked replacement.
- FakeRenderer injects deterministic device loss on the next flush.
- Runtime promotes flush device loss into the existing recovery transaction.
- FakeRenderer scenarios pass 25/25.
- Session scenarios pass 19/19 and Runtime scenarios pass 14/14.
- CTest passes 11/11 executables and cumulative gates pass 9/9.
