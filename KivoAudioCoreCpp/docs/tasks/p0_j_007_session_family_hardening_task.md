# P0-J-007 Session Family Hardening

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-007-SESSION-FAMILY-HARDENING
**Status:** COMPLETED
**Execution Basis:** Runtime genealogy iron rule
**Last Updated:** 2026-06-12

## Purpose

Remove the inherited flat Session family and split the 437-line controller
implementation and 441-line test executable by stable responsibility before
adding EOS and drain behavior.

## Natural Family Tree

```text
include/kivo/core/playback/session/
  command/
    playback_command.hpp
    playback_command_failure.hpp
    playback_command_result.hpp
  controller/
    playback_session_controller.hpp
  snapshot/
    playback_session_snapshot.hpp

src/core/playback/session/
  command/
    playback_session_command.cpp
  controller/
    playback_session_controller.cpp
  position/
    playback_session_position.cpp
  recovery/
    playback_session_recovery.cpp
  result/
    playback_session_result.cpp
  seek/
    playback_session_seek.cpp
  snapshot/
    playback_session_snapshot.cpp
  state/
    playback_session_state.hpp

tests/playback_session/
  fixture/
    session_test_assert.hpp
    session_test_factory.hpp
    scripted_recovery_operations.hpp
  runner/
    playback_session_test_cases.hpp
    playback_session_tests_main.cpp
  scenario/
    command_lifecycle_tests.cpp
    recovery_executor_tests.cpp
    recovery_state_tests.cpp
    seek_tests.cpp
```

## Responsibility Rules

- Public command values own only command input/outcome vocabulary.
- Controller exposes orchestration entry points only.
- Snapshot owns immutable observation state only.
- Internal state owns synchronized mutable fields and method declarations.
- Each implementation branch defines one behavior axis.
- Fixtures contain reusable deterministic doubles only.
- Scenario files contain assertions for one behavior family only.
- Runner owns registration and reporting only.

## Non-Goals

- No state-machine behavior changes.
- No new command kind.
- No EOS or drain implementation.
- No recovery policy changes.
- No generic helper, utility, detail, common, or miscellaneous directory.

## Gates

- Session roots may not contain directly flattened source files.
- Immediate subfamilies must match this declared tree.
- Session production and test files must not exceed 260 lines.
- All moved public headers must compile independently.
- Existing tests and all cumulative gates must remain green.

## Exit Criteria

- The 437-line implementation and 441-line test files no longer exist.
- Maximum Session production/test source is below 260 lines.
- All includes use the owning subfamily path.
- MSVC `/W4 /WX`, CTest, and all gates pass with no behavior regression.

## Completion Evidence

- The former flat 437-line implementation and 441-line test file were removed.
- Session family roots contain subfamilies only; no source file is flattened at
  the public, implementation, or test root.
- Maximum production file size is 203 lines.
- Maximum test file size is 117 lines.
- Old flat Session include paths have zero references.
- MSVC Debug build passed with `/W4 /WX`.
- CTest passed 11 of 11 test executables.
- All 9 cumulative audio-core gates passed.
