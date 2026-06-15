# P0-K-002 WASAPI Family Hardening

**Task:** KIVO-CPP-AUDIO-CORE-P0-K-002-WASAPI-FAMILY-HARDENING
**Status:** COMPLETED
**Execution Basis:** Genealogy iron rule and P0-K structural audit
**Last Updated:** 2026-06-12

## Finding

The WASAPI renderer implementation had reached 734 lines and combined endpoint
discovery, format negotiation, client initialization, lifecycle control,
draining, writes, failure translation, diagnostics, and the public facade.
The render-worker implementation had also reached 281 lines. Public headers,
platform helpers, and unit tests were flat.

## Natural Family Tree

```text
include/kivo/platform/windows/wasapi/
  diagnostics/
  renderer/
  result/
  worker/

src/platform/windows/wasapi/
  apartment/
  error/
  format/
  renderer/
  worker/

tests/platform_windows/wasapi/
  fixture/
  runner/
  scenario/
```

## Responsibility Rules

- Apartment owns COM initialization and owner-thread release.
- Error owns HRESULT classification.
- Format owns WAVEFORMATEX translation.
- Renderer open owns endpoint discovery, negotiation, and stream initialization.
- Renderer control owns start, flush, reset, stop, and close transitions.
- Renderer drain owns bounded hardware-buffer draining.
- Renderer write owns generation validation and buffer submission.
- Renderer failure owns terminal mapping and resource release.
- Renderer query owns snapshots, capabilities, waits, and diagnostics.
- Renderer facade owns only public boundary forwarding.
- Worker lifecycle owns thread creation, stop, join, and snapshots.
- Worker run owns MMCSS registration and the render-pump loop.
- Test fixtures, runners, and scenarios remain separately owned.

## Gates

- Public, source, and unit-test roots contain zero direct files.
- Immediate subfamilies match the declared tree exactly.
- Every production and unit-test file remains at or below 260 lines.
- Old flat WASAPI header, source, and unit-test paths have zero references.
- WASAPI tests and dependent FFmpeg/WASAPI targets build without behavior drift.
- Full build, CTest, and cumulative repository gates pass.

## Completion Evidence

- Public, source, and unit-test roots contain zero direct files.
- Old flat WASAPI header, source, and unit-test paths have zero references.
- Largest WASAPI production file is 227 lines.
- The former 281-line worker implementation is split; its largest unit is 131 lines.
- Largest WASAPI unit-test file is 144 lines.
- WASAPI adapter scenarios pass 14/14.
- WASAPI unit, tone-smoke, and FFmpeg/WASAPI integration targets build.
- MSVC Debug full build passes with `/W4 /WX`.
- CTest passes 11/11 executables.
- Cumulative repository gates pass 9/9.
