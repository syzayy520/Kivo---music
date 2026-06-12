# P0-J-013 Active Operation Shutdown

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-013-ACTIVE-OPERATION-SHUTDOWN
**Status:** COMPLETED
**Execution Basis:** P0-J shutdown and close race requirements
**Last Updated:** 2026-06-12

## Purpose

Prove that Shutdown and CloseSource serialize behind active renderer boundary
calls, then release decoder, queue, producer, and renderer ownership exactly
once.

## Natural Family Tree

```text
tests/playback_runtime/coordinator/
  fixture/
    runtime_test_call_gate.hpp
    runtime_test_gated_renderer.hpp
  scenario/
    active_operation_shutdown_tests.cpp
```

## Responsibility Rules

- Call gate owns deterministic entry/release synchronization only.
- Gated renderer owns delegation plus optional write/drain blocking only.
- Scenario tests own Runtime ordering and final ownership assertions only.
- Production Runtime locking remains unchanged unless the proof finds a defect.

## Required Proofs

- Shutdown waits behind an active render write.
- CloseSource waits behind an active drain.
- No wall-clock sleep is used for synchronization.
- Decoder and renderer close once after the active call completes.
- Runtime and Session reject mutation after terminal Shutdown.

## Gates

- Fixture and scenario files remain below 260 lines.
- Runtime test family remains free of flattened files.
- Thread tests complete deterministically without timeout polling.
- MSVC `/W4 /WX`, all CTest executables, and all cumulative gates pass.

## Completion Evidence

- Shutdown remains incomplete while a renderer write is held at the gate.
- CloseSource remains incomplete while renderer drain is held at the gate.
- Releasing each gate allows the active call to finish before close ownership.
- Decoder closes once, renderer reaches Closed, and Runtime becomes inactive.
- Post-Shutdown mutation is rejected as `RuntimeClosed`.
- Synchronization uses condition variables and latches with no sleeps.
- Largest new test file is 150 lines.
- Runtime scenarios pass 12/12.
- CTest passes 11/11 executables and cumulative gates pass 9/9.
