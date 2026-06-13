# P0-K-009 Abnormal Media and Device Power Closeout

**Task:** KIVO-CPP-AUDIO-CORE-P0-K-009-ABNORMAL-MEDIA-DEVICE-POWER
**Status:** COMPLETED
**Execution Basis:** P0-K hostile-media and device-transition requirements
**Last Updated:** 2026-06-12

## Goal

Close the remaining implementation gaps in the hostile-media corpus and the
Windows device matrix without representing absent hardware as passed.

## Natural Family Tree

```text
tests/ffmpeg_decode/
  fixture/
    assertion/
    decode/
    harness/
    mutation/
  scenario/
    control/
    corruption/
    cue/
    decode/
    policy/
    truncation/

tests/hardware/device_matrix/platform/windows/
  apartment/
  driver/
  endpoint/

src/platform/windows/wasapi/
  device/notification/
  power/notification/
  renderer/
```

## Responsibility Gate

- Mutation fixtures create deterministic temporary derivatives only.
- Abnormal-media assertions classify bounded decode outcomes only.
- Corruption scenarios own header damage and trailing-junk behavior.
- Truncation scenarios own depth and container coverage.
- PnP probing owns endpoint-to-parent-driver correlation.
- Endpoint metadata and active capability probes remain separate.
- Power callbacks publish atomic transition observations only.
- Renderer control consumes observations and applies recovery state.

## Implemented Behavior

- One-third and two-thirds truncation across WAV, FLAC, CBR MP3, VBR MP3,
  and AAC/M4A.
- Destroyed-header classification for all supported corpus families.
- MP3 resynchronization is accepted only when decoded frames are produced.
- Appended deterministic junk reaches EOS or `InvalidMediaData`; it cannot
  become a boundary failure or an unbounded decode loop.
- All render endpoint states are inventoried, not only active endpoints.
- Active endpoint capability probing is separated from inactive metadata.
- Endpoint parent driver name, provider, and version are correlated through
  SetupAPI.
- Suspend and resume notifications publish a one-shot renderer invalidation
  and require reopen on the next operation.

## Verification

- Full FFmpeg-enabled MSVC build: passed.
- FFmpeg adapter scenarios: 16 of 16 passed.
- WASAPI adapter scenarios: 19 of 19 passed.
- CTest groups: 12 of 12 passed.
- Real FLAC-to-WASAPI playback: 96,000 accepted frames, zero underruns,
  timeouts, and invalidations.
- Device inventory gate: passed with 12 endpoint records.
- Repository gates: all 9 passed.
- Largest C/C++ source: 255 lines.

## External Evidence Boundary

USB DAC, Bluetooth, active HDMI/DisplayPort playback, physical default-device
switch, removable-device unplug/replug, real system sleep/resume, and a
10-hour release-candidate run require matching physical hardware or an
owner-approved release laboratory. They remain explicit P0-P release evidence
and are not represented as implementation failures or false passes.
