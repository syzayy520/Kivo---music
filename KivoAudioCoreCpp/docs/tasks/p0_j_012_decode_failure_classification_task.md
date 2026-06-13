# P0-J-012 Decode Failure Classification

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-012-DECODE-FAILURE-CLASSIFICATION
**Status:** COMPLETED
**Execution Basis:** P0-J recovery and fault-isolation requirements
**Last Updated:** 2026-06-12

## Purpose

Preserve source, codec, media, format, and internal failure truth when decode
production fails, then execute only recovery fallbacks owned by the audio core.

## Natural Family Tree

```text
include/kivo/core/playback/recovery/
  classification/
    decode_failure_domain.hpp

src/core/playback/runtime/recovery/
  playback_runtime_decode_failure.cpp
  playback_runtime_device_recovery.cpp

tests/playback_session/scenario/
  decode_failure_classification_tests.cpp

tests/playback_runtime/coordinator/scenario/
  decode_failure_recovery_tests.cpp
```

## Responsibility Rules

- Classification maps `DecodeFailure` to `ErrorDomain` without side effects.
- Runtime recovery owns decode-failure policy execution and counters only.
- Session failure reporting records an already-classified error domain.
- Source-unavailable fallback may stop the current media while retaining
  explicit close ownership.
- Codec skip-track remains outside the audio core because playlist ownership is
  not present here; the runtime must fail truthfully instead of fabricating a
  successful skip.

## Failure Policy

- Source open/read/seek failure maps to `SourceUnavailable`.
- Unsupported codec and decoder-open failure map to `CodecError`.
- Invalid media maps to `MediaError`.
- Unsupported source/container/stream and required conversion map to
  `UnsupportedFormat`.
- Conversion failure maps to `FormatError`.
- Contract, stale-generation, invalid-buffer, and impossible EOS failures map
  to `InternalBug`.

## Gates

- Recovery roots remain free of directly flattened files.
- Classification and Runtime recovery files remain below 260 lines.
- Source fallback stops renderer and Session deterministically.
- Codec failure records failed recovery and remains explicitly closable.
- MSVC `/W4 /WX`, all CTest executables, and all cumulative gates pass.

## Exit Criteria

- Decode production failure never loses its error domain.
- Runtime snapshot exposes bounded decode-recovery observations.
- No recovery path reports success for an action the core cannot perform.

## Completion Evidence

- Every current `DecodeFailure` value has an asserted domain mapping.
- Source failure in prebuffer state stops media and remains explicitly closable.
- Codec failure records a failed recovery without fabricating playlist action.
- Seek and device-recovery source failures retain `SourceUnavailable`.
- Recovery, Runtime, Session, and test roots contain zero direct files.
- Largest touched Runtime or Recovery production file is 160 lines.
- MSVC Debug build passes with `/W4 /WX`.
- Session scenarios pass 18/18 and Runtime scenarios pass 10/10.
- CTest passes 11/11 executables and cumulative gates pass 9/9.
