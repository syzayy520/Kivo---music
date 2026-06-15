# P0-D-018 Contract Closeout Hardening

**Task:** KIVO-CPP-AUDIO-CORE-P0-D-018-CONTRACT-CLOSEOUT-HARDENING  
**Status:** COMPLETE  
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`  
**Owner Direction:** Continue implementation cycles without planning-only delay  
**Last Updated:** 2026-06-12

## Purpose

Close the remaining P0-C correctness and validation gaps before the render
boundary and deterministic fake backend become implementation dependencies.

The existing isolated tests were not sufficient evidence because public
headers could conflict when composed, format truth did not fully carry layout
semantics, and ordinary PowerShell validation could not initialize MSVC.

## Scope

This task covers:

- Completion of the realtime buffer-lifetime type rename.
- Public-header standalone and aggregate composition checks.
- PCM valid/container-bit, frame-layout, and channel-mask truth.
- `StatusOr<T>` construction safety and direct includes.
- Ordinary PowerShell MSVC environment initialization.
- Clean build-directory handling.
- Validation target and gate authorization.

## Non-Goals

- No render boundary implementation.
- No fake backend implementation.
- No WASAPI or FFmpeg code.
- No playback thread.
- No product feature expansion.

## Natural Family Tree

```text
include/kivo/core/contract/
  format/descriptor/
    audio_format_descriptor.hpp
    channel_mask.hpp
  realtime/
    buffer/buffer_lifetime_rule.hpp
    boundary/realtime_boundary_contract.hpp
  result.hpp

tests/contracts/
  composability/
    public_header_composability_tests.cpp
  format/descriptor/
    audio_format_descriptor_tests.cpp
  foundation/
    result_tests.cpp
  realtime/boundary/
    realtime_boundary_contract_tests.cpp

tools/
  validation/
    import_msvc_environment.ps1
    environment_probe.ps1
    windows_toolchain_locator.ps1
    build_validation_entry.ps1
    git_state_probe.ps1
  gates/
    check_forbidden_token_gate.ps1
    check_no_audio_runtime_gate.ps1

CMakeLists.txt
CMakePresets.json
```

Each file remains with its owning family. No generic helper, utility, common,
or manager directory is introduced.

## Design Decisions

### Realtime lifetime naming

`BufferLifetimeProof` describes the general storage lifetime model.
`RealtimeBufferLifetimeProof` describes proof requirements at the realtime
boundary. They are distinct concepts and must have distinct public names.

### Audio format truth

`AudioFormatDescriptor` carries:

- Sample encoding.
- Named channel layout.
- Interleaved or planar frame layout.
- Optional explicit platform-neutral channel mask.
- Sample rate.
- Valid bits.
- Container bits.

Implicit container bits and channel masks normalize to the values defined by
the selected sample format and named layout. Explicit contradictory values are
invalid. Frame bytes use container bits.

### Generic result construction

`StatusOr<T>` has no invalid default state and does not require `T` to be
default constructible. Variant alternatives are accessed by index so
`StatusOr<ErrorInfo>` remains representable.

### Header compilation

CMake generates one validation translation unit per public contract header and
one translation unit containing every public contract header. These compile as
an object-only validation target and are dependencies of both test
executables.

### Toolchain initialization

Validation discovers Visual Studio with `vswhere`, imports the x64
`vcvarsall.bat` environment into the current validation process, and falls
back to known installation roots only when discovery is unavailable.

Generated build directories are normalized, required to remain inside
`PROJECT_ROOT`, and removed before clean validation.

## Static Structure Checks

- No duplicate public type name.
- No platform token in core contracts.
- No generated source outside ignored build directories.
- Header validation target is object-only.
- No runtime target introduced.
- No source file outside authorized project root.

## Validation Requirements

1. Ordinary PowerShell environment probe finds MSVC.
2. Clean Ninja configure succeeds.
3. Every public contract header compiles independently.
4. All public contract headers compile together.
5. Contract and capability targets build with warnings as errors.
6. Contract and capability tests pass.
7. All repository gates pass.
8. `git diff --check` passes.
9. Clean branch validation uses the branch upstream rather than requiring
   equality with `origin/master`.

## Failure Classification

- Header compile failure: source correctness failure.
- Duplicate public declaration: contract composition failure.
- Invalid PCM truth test: audio model failure.
- Missing Visual Studio installation: environment blocker.
- Discovered installation that cannot initialize: validation tooling failure.
- Gate rejection of an authorized validation-only target: gate contract
  failure.

## Exit Criteria

This task closes when all validation requirements pass from a clean branch and
the result is committed. The next implementation task is the minimal
platform-neutral render boundary, followed immediately by deterministic fake
renderer fault injection.

## Validation Record

Final local closeout on 2026-06-12:

- `PASS_TOOLCHAIN_PROBE_READY_NINJA`
- `PASS_BUILD_VALIDATION_NINJA`
- 214 public contract headers compiled independently.
- One aggregate translation unit compiled all 214 public headers together.
- Contract tests: 892/892 passed.
- Capability tests: 68/68 passed.
- CTest: 2/2 targets passed.
- Repository gates: 9/9 passed.
- Duplicate public type-name audit passed.
- `git diff --check` passed.
- Localized MSVC dependency output was decoded correctly and filtered by
  Ninja after the validation entry selected UTF-8 code page 65001.

## Future Phase Ownership

- P0-D/P0-E owns render boundary and deterministic fault injection.
- P0-F/P0-H owns WASAPI adapter and real generated PCM output.
- P0-G/P0-I owns FFmpeg decode and local-file playback.

## Deferred

Static analysis, fuzzing, sanitizers, hardware matrix, gapless, exclusive mode,
DSP, ABI, and release packaging remain owned by their roadmap milestones.
