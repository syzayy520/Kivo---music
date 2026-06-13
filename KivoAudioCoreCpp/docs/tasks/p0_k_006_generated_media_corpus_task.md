# P0-K-006 Generated Media Corpus

**Task:** KIVO-CPP-AUDIO-CORE-P0-K-006-GENERATED-MEDIA-CORPUS
**Status:** COMPLETED
**Execution Basis:** P0-K media corpus and hostile media requirements
**Last Updated:** 2026-06-12

## Corpus Family

All media is generated into the build tree from deterministic `lavfi` sources.
No binary media is committed and no third-party recording is used.

```text
PCM WAV
FLAC
CBR MP3
VBR MP3
AAC in M4A
ALAC in M4A
Vorbis in Ogg
Opus in Ogg
22.05 kHz mono PCM
96 kHz stereo FLAC
48 kHz 5.1 PCM
30-second PCM large-file fixture
severely truncated WAV, FLAC, VBR MP3, and M4A derivatives
```

## Required Behavior

- Probe reports the correct codec, container, native sample rate, and channel
  layout.
- Every valid fixture converts to the requested 48 kHz stereo float format.
- Decoded frame counts remain within explicit fixture bounds.
- Severe truncation never becomes a successful normal end-of-stream result.
- A declared-duration stream that ends materially early is classified as
  `InvalidMediaData`.
- Existing seek, conversion policy, lifecycle, and resource tests remain green.

## Family Gate

- Generated fixtures remain build artifacts only.
- Generator commands, sizes, and SHA-256 hashes are recorded.
- Dependency gate locks every generated output hash.
- FFmpeg decode test files remain at or below 260 lines.

## Completion Evidence

- Twelve deterministic valid fixtures generated and hash-locked.
- Valid decode corpus: PCM WAV, FLAC, CBR MP3, VBR MP3, AAC/M4A, ALAC/M4A,
  Vorbis/Ogg, Opus/Ogg,
  22.05 kHz mono, 96 kHz stereo, 48 kHz 5.1, and 30-second PCM.
- FFmpeg adapter scenarios: 16 of 16 passed after responsibility-based test
  family consolidation.
- One-third and two-thirds truncation cover WAV, FLAC, CBR MP3, VBR MP3, and
  AAC/M4A.
- Destroyed-header and deterministic trailing-junk derivatives cover every
  supported corpus family.
- Severe WAV truncation rejected by RIFF declared-extent validation.
- Severe FLAC, VBR MP3, and M4A truncation classified as
  `InvalidMediaData`.
- CUE sheet input explicitly rejected as `UnsupportedContainer`.
- Local-file source validated with a sparse file of
  `4,294,975,488` bytes, including exact seek and marker read beyond the
  32-bit boundary.
- Decode boundary scenarios: 10 of 10 passed.
- Largest changed FFmpeg or decode-boundary source: 246 lines.
- Full FFmpeg-enabled build: passed.
- CTest: 12 of 12 test groups passed.
- Repository gate validation: all 9 gates passed.
- Dependency/license gate confirms all generated media hashes.
