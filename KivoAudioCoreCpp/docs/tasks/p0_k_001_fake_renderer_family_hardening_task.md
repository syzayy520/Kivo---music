# P0-K-001 Fake Renderer Family Hardening

**Task:** KIVO-CPP-AUDIO-CORE-P0-K-001-FAKE-RENDERER-FAMILY-HARDENING
**Status:** COMPLETED
**Execution Basis:** Genealogy iron rule and P0-K structural audit
**Last Updated:** 2026-06-12

## Finding

The deterministic renderer implementation had reached 470 lines and mixed
construction, lifecycle control, writes, fault scheduling, consumption,
observation, and timing conversion. The test family also retained flat files.
That shape does not satisfy single-file single-responsibility.

## Natural Family Tree

```text
include/kivo/testing/render/
  configuration/
  fault/
  observation/
  renderer/

src/testing/render/
  consumption/
  fault/
  lifecycle/
  observation/
  write/

tests/fake_renderer/
  fixture/
  runner/
  scenario/
```

## Responsibility Rules

- Renderer owns the public deterministic boundary facade.
- Lifecycle owns open and control-state transitions.
- Write owns validation, generation rejection, capacity, and accepted frames.
- Fault owns scheduling and deterministic trigger consumption.
- Consumption owns simulated device progress and frame-duration conversion.
- Observation owns snapshots, counters, and bounded event recording.
- Test fixtures, runners, and scenarios remain separately owned.

## Gates

- The three family roots contain zero direct files.
- Immediate subfamilies match the declared tree exactly.
- Every production and test file in this family remains at or below 260 lines.
- Old flat header, source, and test paths have zero references.
- Fake Renderer and Playback Runtime behavior remains unchanged.
- Full build, CTest, and cumulative repository gates pass.

## Completion Evidence

- Public, source, and test roots contain zero direct files.
- Old flat Fake Renderer header, source, and test paths have zero references.
- Largest Fake Renderer implementation file is 134 lines.
- Largest Fake Renderer test file is 201 lines.
- Fake Renderer scenarios pass 25/25.
- Playback Runtime scenarios pass 14/14.
- MSVC Debug full build passes with `/W4 /WX`.
- CTest passes 11/11 executables.
- Cumulative repository gates pass 9/9.
