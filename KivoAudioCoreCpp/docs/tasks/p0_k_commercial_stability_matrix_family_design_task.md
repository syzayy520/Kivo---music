# P0-K Commercial Stability Matrix

**Status:** ACTIVE IMPLEMENTATION
**Execution Authority:** Owner-directed continuous implementation
**Last Updated:** 2026-06-12

## Implemented Automated Evidence

- Deterministic renderer lifecycle and bounded observation history.
- Repeated FFmpeg open/decode/close resource measurements.
- Windows process handle and PrivateUsage growth assertions.
- PCM WAV, FLAC, CBR MP3, VBR MP3, and AAC/M4A.
- 22.05 kHz mono, 44.1/48/96 kHz stereo, and 48 kHz 5.1 input.
- 30-second generated PCM fixture and a sparse source beyond 4 GiB.
- Severe truncation for WAV, FLAC, VBR MP3, and M4A.
- Random corrupt input classification.
- CUE sheet input explicitly unsupported.
- Queue pressure, backpressure, starvation, seek generation, track-switch
  race, device-loss recovery, and lifecycle shutdown tests.
- Real generated PCM WASAPI output.
- Real FFmpeg-to-SPSC-to-MMCSS-to-WASAPI playback.
- One-hour real-device endurance evidence.
- Executable active-render-endpoint inventory with default-role, mix-format,
  shared/exclusive support, form-factor, and device-period evidence.

## Natural Validation Families

```text
tests/
  decode_boundary/
    fixture/
    runner/
    scenario/
  ffmpeg_decode/
    fixture/
    runner/
    scenario/
  hardware/
    device_matrix/
      fixture/
      platform/
        windows/
          apartment/
          endpoint/
      runner/
      scenario/
    decode_output/
      fixture/
      runner/
      scenario/
    wasapi/
      fixture/
      runner/
      scenario/
  stability/
    fixture/
    platform/
      windows/
    runner/
    scenario/
```

## Remaining P0-K Evidence

- Physical device matrix still missing USB DAC, Bluetooth, and active
  HDMI/DisplayPort evidence. The current onboard endpoint is inventoried and
  covered by real playback plus one-hour endurance evidence.
- Default-device change and unplug/replug on applicable hardware.
- Sleep/resume on the release candidate build.
- Ten-hour release-candidate endurance run.
- Per-device identity, driver, format, mode, underrun, recovery, and
  limitation records.

P0-K remains open until the remaining physical-lab evidence is recorded. The
automated matrix is implemented and must remain green while the physical matrix
is completed.
