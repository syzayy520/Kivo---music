# P0-Q Hi-Res, DSD, and DoP Research Notes

**Date:** 2026-06-12
**Status:** HI-RES PCM AND DSD/DOP CONTRACT SLICES COMPLETE

## Findings

- Hi-Res PCM is an extension of existing PCM truth only when sample rate,
  valid bits, container bits, channel layout, output mode, device format,
  conversion, processing, and engine participation are all known.
- Native DSD requires a non-PCM stream contract. It must not be represented by
  the current PCM `SampleFormat` enum.
- DoP requires a carrier contract that distinguishes PCM container transport
  from DSD payload. The PCM carrier is not user-audible PCM content for DSP.
- DSD-to-PCM decode or conversion may be useful, but it is a conversion path
  and must reject native DSD or DoP passthrough claims.
- Runtime device probing is mandatory before any native DSD or DoP claim.

## Current Product Claim Impact

No current product claim changes.

Forbidden claims remain:

- Hi-Res playback support.
- Native DSD support.
- DoP passthrough support.
- SACD support.
- DSD bit-perfect support.

## Primary References

- DoP Open Standard v1.1:
  `https://dsd-guide.com/sites/default/files/white-papers/DoP_openStandard_1v1.pdf`
- FFmpeg formats:
  `https://ffmpeg.org/ffmpeg-formats.html`
- FFmpeg codecs:
  `https://ffmpeg.org/ffmpeg-codecs.html`
- Microsoft WASAPI:
  `https://learn.microsoft.com/en-us/windows/win32/coreaudio/wasapi`

## Next Research Slice

Recommended first implementation-adjacent slice:

```text
Hi-Res PCM validation and truthful rejection/acceptance matrix
```

Reason:

- It reuses existing PCM truth families.
- It avoids native DSD hardware assumptions.
- It can strengthen current output-truth and device-evidence reporting without
  adding a new stream class.

Required evidence:

- 88.2, 96, 176.4, 192, and 384 kHz PCM descriptors.
- 24-in-32 and 32-bit integer validity tests.
- WASAPI shared and exclusive format probing rows.
- Processing and bit-perfect truth review.
- Commercial claim review remains rejected until release-lab evidence exists.

## Completed Local Evidence

The repository now includes `hires_pcm_tests`, covering:

- 88.2, 96, 176.4, 192, and 384 kHz PCM descriptors.
- 24-in-32 valid-bit versus container-bit truth.
- Container-bit based frame-size calculations.
- Current PCM sample formats remaining integer or float PCM only.
- Rejection of non-PCM payload attempts through the PCM descriptor.

This is contract-level evidence only. Device support and commercial Hi-Res
claims remain unapproved until WASAPI probing and release-lab records exist.

The repository now includes DSD/DoP value-only contract tests, covering:

- DSD64, DSD128, DSD256, and DSD512 rate identities.
- Native DSD requiring a known DSD rate and bounded channel count.
- DoP marker pair validation.
- DoP carrier sample-rate derivation from the DSD payload rate.
- DoP rejection for wrong carrier rate, wrong container width, and bad markers.

This is contract-level evidence only. Decode, render, endpoint, passthrough,
SACD, and commercial DSD/DoP claims remain unapproved.

The repository now includes decode/media support classification tests, covering:

- Existing PCM-backed codecs remain supported by the decode boundary.
- Native DSD in DSF or DSDIFF is recognized but explicitly unsupported.
- DoP carrier input is explicitly unsupported even when carried in PCM-shaped
  data.
- Unknown codec and unknown container failures remain distinct.

This is boundary evidence only. It prevents accidental support claims and does
not implement DSD/DoP decode or render.
