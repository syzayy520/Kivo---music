# P0-F/H-001 WASAPI Shared-Mode Output

**Task:** KIVO-CPP-AUDIO-CORE-P0-FH-001-WASAPI-SHARED-OUTPUT
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Boundary Basis:** `docs/tasks/p0_e_001_platform_neutral_render_boundary_task.md`
**Last Updated:** 2026-06-12

## Purpose

Implement the first production Windows output adapter and prove audible
generated PCM through the default render endpoint. The adapter implements the
same `RenderBoundary` already proven by the fake renderer.

## Base Gate

- P0-E-001 render boundary is complete.
- P0-D-019 deterministic fake renderer and first fault set are complete.
- Windows SDK Core Audio headers and x64 import libraries are present.
- MSVC/Ninja clean validation passes.

## Official API Basis

- `IAudioClient::GetMixFormat`
  <https://learn.microsoft.com/windows/win32/api/audioclient/nf-audioclient-iaudioclient-getmixformat>
- `IAudioClient::Initialize`
  <https://learn.microsoft.com/windows/win32/api/audioclient/nf-audioclient-iaudioclient-initialize>
- WASAPI event callback stream flag
  <https://learn.microsoft.com/windows/win32/coreaudio/audclnt-streamflags-xxx-constants>
- `IAudioRenderClient::GetBuffer`
  <https://learn.microsoft.com/windows/win32/api/audioclient/nf-audioclient-iaudiorenderclient-getbuffer>
- `IAudioRenderClient::ReleaseBuffer`
  <https://learn.microsoft.com/windows/win32/api/audioclient/nf-audioclient-iaudiorenderclient-releasebuffer>
- Default render endpoint
  <https://learn.microsoft.com/windows/win32/api/mmdeviceapi/nf-mmdeviceapi-immdeviceenumerator-getdefaultaudioendpoint>

## Natural Family Tree

```text
include/kivo/platform/windows/wasapi/
  wasapi_diagnostics.hpp
  wasapi_wait_result.hpp
  wasapi_renderer.hpp

src/platform/windows/wasapi/
  com_apartment.hpp
  com_apartment.cpp
  wasapi_error.hpp
  wasapi_error.cpp
  wasapi_format.hpp
  wasapi_format.cpp
  wasapi_renderer.cpp

tests/platform_windows/wasapi/
  wasapi_tests_main.cpp
  wasapi_tests_main.hpp
  format_mapping_tests.cpp
  error_mapping_tests.cpp

tests/hardware/wasapi/
  wasapi_tone_smoke.cpp
```

Public adapter headers contain no COM interface, `HANDLE`, `HRESULT`,
`WAVEFORMATEX`, or Windows header. The implementation owns all platform types.

## Adapter Responsibilities

- Initialize and balance the calling thread's COM apartment.
- Resolve the default console render endpoint.
- Derive stable renderer identity from endpoint identity.
- Query shared-mode mix format.
- Map `WAVEFORMATEX`/`WAVEFORMATEXTENSIBLE` into `RenderFormat`.
- Initialize event-driven shared-mode `IAudioClient`.
- Expose truthful accepted format and buffer capacity.
- Copy accepted frames into `IAudioRenderClient` buffers without allocation.
- Map device invalidation and buffer errors into `RenderFailure`.
- Track device generation, underruns, invalidations, and last platform code.
- Expose truthful shared/exclusive adapter support and actual active mode.
- Expose default/minimum device period and actual endpoint buffer duration.
- Mark device-invalidated state as requiring reopen.
- Release event, render client, audio client, endpoint, and COM ownership on
  every close or failure path.

## Initial Support

- Windows 10 or newer, x64.
- Default `eRender` / `eConsole` endpoint.
- Shared mode.
- Event-driven buffering.
- Interleaved PCM integer or IEEE float mix formats that map exactly to the
  core descriptor.
- Exclusive mode is reported as unsupported, never as successful.
- No FFmpeg, file I/O, host callbacks, UI, or product state machine.

## Lifecycle

- `open` activates the endpoint and negotiates the shared mix format.
- `start` starts the audio client.
- `write` reads current padding, writes the available prefix, and reports
  full, partial, delayed, or device-lost failure.
- `flush` performs stop/reset and adopts new generations.
- `drain` waits on the event handle until padding reaches zero or timeout.
- `reset` performs stop/reset and returns to Open.
- `stop` stops the client and returns to Stopped.
- `close` releases all platform objects and returns to Closed.

Control operations must execute on the COM-owning control thread. The render
write call may execute on the render thread while control is serialized.
Concurrent control mutation is deferred to the product-state milestone.

## Realtime Rules

- No allocation in steady-state `write`.
- No blocking wait in `write`.
- No logging or string formatting in `write`.
- No FFmpeg call.
- No host callback.
- `memcpy` only after exact frame-byte validation.
- Platform failure mapping uses numeric codes and fixed value types.

## Generated PCM Hardware Proof

`kivo_wasapi_tone_smoke`:

- Opens the default endpoint with compatible-format negotiation.
- Allocates one reusable PCM block before start.
- Generates a bounded-amplitude sine wave in the accepted mix format.
- Waits on the WASAPI event outside `write`.
- Handles partial writes by reusing `AudioBlockView::tail_after`.
- Runs for a configurable duration.
- Supports zero/low-amplitude long-run stability mode without changing the
  device submission path.
- Repeats start/write/drain/reset on one device instance with `--cycles`.
- Stops, resets, closes, and prints diagnostics.
- Returns nonzero on unsupported format, missing device, device loss, or
  lifecycle failure.

The smoke executable is hardware-dependent and is built by default but not
registered as an unconditional CTest.

## Tests

- PCM/float Windows format mapping.
- Unsupported compressed or malformed format rejection.
- HRESULT mapping for device invalidation, timeout, buffer size, and generic
  boundary failure.
- Public header platform quarantine.
- Default device open/close hardware probe.
- Audible generated tone.
- Repeated start/stop.
- Explicit unsupported exclusive-mode truth.

## Gate Changes

- Authorize Windows audio API tokens only inside
  `src/platform/windows/wasapi/`, `tests/platform_windows/wasapi/`, and
  `tests/hardware/wasapi/`.
- Continue forbidding Windows types in core and public adapter headers.
- Authorize only `kivo_wasapi_renderer`, `kivo_wasapi_tests`, and
  `kivo_wasapi_tone_smoke`.
- Continue forbidding FFmpeg, third-party dependency, demo, and UI targets.

## Stop Conditions

- A Windows type reaches `include/kivo/core/` or a public adapter header.
- Shared mode reports success without a valid accepted format.
- `write` allocates, waits, logs, or calls back into the host.
- A failed open leaves a COM object or event handle alive.
- Unsupported exclusive mode reports success.
- Hardware smoke cannot distinguish no-device environment from source
  correctness failure.

## Exit Criteria

- Adapter shell and format/error mapping tests pass.
- Default device opens and closes on available hardware.
- Audible generated PCM reaches the default endpoint.
- Start/stop/reset cycles are repeatable.
- Diagnostics truthfully report mode, generation, underrun, invalidation, and
  last platform failure.
- A 60-minute real-device run submits and accepts 172,800,000 frames with zero
  worker wait timeouts, underruns, and device invalidations.
- Full cumulative validation passes.
