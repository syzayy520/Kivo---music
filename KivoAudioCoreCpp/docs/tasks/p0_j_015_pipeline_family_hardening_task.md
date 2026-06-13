# P0-J-015 Pipeline Family Hardening

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-015-PIPELINE-FAMILY-HARDENING
**Status:** COMPLETED
**Execution Basis:** Genealogy iron rule and P0-J structural closeout
**Last Updated:** 2026-06-12

## Natural Family Tree

```text
include/kivo/core/playback/pipeline/
  configuration/
  producer/
  result/
  snapshot/

src/core/playback/pipeline/
  buffer/
  producer/
  state/
  step/
  transfer/

tests/playback_pipeline/
  fixture/
  runner/
  scenario/
```

## Responsibility Rules

- Buffer owns copied decoded-block storage vocabulary.
- State owns producer state and member declarations.
- Step owns decode lookahead and terminal dispatch.
- Transfer owns validation, chunking, queue transfer, and backpressure.
- Producer owns construction, allocation, and the public facade.
- Test fixture, runner, and scenarios remain separately owned.

## Gates

- Pipeline roots contain zero direct files.
- Immediate subfamilies match this declared tree.
- Every Pipeline production and test file remains below 260 lines.
- Behavior remains unchanged.
- Full build, CTest, and cumulative gates pass.

## Completion Evidence

- Public, source, and test Pipeline roots contain zero direct files.
- Old flat Pipeline include, source, and test paths have zero references.
- Largest Pipeline production file is 161 lines.
- Largest Pipeline test file is 75 lines.
- Pipeline scenarios pass 7/7 and Runtime scenarios pass 14/14.
- MSVC Debug build passes with `/W4 /WX`.
- CTest passes 11/11 executables and cumulative gates pass 9/9.
