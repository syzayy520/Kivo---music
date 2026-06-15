# P0-M Exclusive and Bit-Perfect Truth Closeout

**Status:** IMPLEMENTATION COMPLETE
**Date:** 2026-06-12

## Delivered

- Platform-neutral Shared/Exclusive render requests with Shared as default.
- Independent event-driven WASAPI shared and exclusive initialization paths.
- Exact exclusive-format probing with aligned-buffer retry and no fallback.
- Extensible WAVE format mapping in both directions.
- Actual output-mode diagnostics and explicit exclusive capability facts.
- Complete-evidence bit-perfect evaluation with stable rejection categories.
- Deterministic tests for every required mode, format, engine, conversion, and
  processing fact.

## Hardware Truth

The active integrated endpoint supports its 48 kHz stereo Float32 mix format
in shared mode but not in exclusive mode. Shared playback completed with zero
underruns and zero invalidations. The exclusive request was rejected with
`AUDCLNT_E_UNSUPPORTED_FORMAT`; no shared fallback occurred.

This result proves truthful negotiation on the available machine. It does not
claim exclusive or bit-perfect capability for USB, Bluetooth, HDMI, or other
untested endpoints.

## Verification

- Output Truth: 7 of 7 passed.
- Contracts: 892 of 892 passed.
- Render Boundary: 22 of 22 passed.
- Fake Renderer: 26 of 26 passed.
- WASAPI Adapter: 21 of 21 passed.
- Public-header standalone and aggregate compilation: passed.
- Repository genealogy and runtime authorization gates: passed.
- Largest P0-M truth file: 147 lines.
- Largest new WASAPI open-family file: 100 lines.

## Next Phase

P0-N owns software volume, ReplayGain, resampling, channel remix, DSP
participation, latency/tail reporting, dither policy, and strict bypass.
