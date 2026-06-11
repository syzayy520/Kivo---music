# Generated Audio Test Fixtures

**Purpose:** Deterministic local decode and hardware-loop validation
**Generator:** Approved FFmpeg LGPL shared baseline
**Generated:** Build output only; never committed as binary media
**Last Updated:** 2026-06-12

## Source and Licensing

All fixtures are synthesized sine waves produced by FFmpeg's `lavfi` sine
source. They contain no third-party recording, performance, artwork, metadata,
or user data.

The exact generator binary and configuration are recorded in
`ffmpeg_lgpl_shared_7_1.md`.

## Commands

```text
ffmpeg -f lavfi -i sine=frequency=523.25:duration=2:sample_rate=44100 -ac 2 -c:a pcm_s16le tone_pcm_s16le.wav
ffmpeg -f lavfi -i sine=frequency=659.25:duration=2:sample_rate=44100 -ac 2 -c:a flac tone.flac
ffmpeg -f lavfi -i sine=frequency=783.99:duration=2:sample_rate=44100 -ac 2 -c:a libmp3lame -b:a 192k tone.mp3
```

## Locked Outputs

For FFmpeg `n7.1.4-39-ga5faeca88f-20260611`:

| File | Size | SHA-256 |
|---|---:|---|
| `tone_pcm_s16le.wav` | 352,878 | `101b74ef127db194060ca1240e16551f92d3449d75f0248c3894963290f53de1` |
| `tone.flac` | 33,563 | `0239214dc0f0a21376185d71a482f4f0b1b704dc315a5432d2acb2d23783ce46` |
| `tone.mp3` | 49,571 | `a111e9605c587b55f99c43998d86220ea234c15c12b6b31ed29b015a8a744a42` |

Hash drift means the generator revision, command, codec configuration, or
container metadata changed and must be reviewed before the baseline is updated.
