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
ffmpeg -f lavfi -i sine=frequency=880:duration=2:sample_rate=44100 -ac 2 -c:a libmp3lame -q:a 2 tone_vbr.mp3
ffmpeg -f lavfi -i sine=frequency=987.77:duration=2:sample_rate=44100 -ac 2 -c:a aac -b:a 160k -movflags +faststart tone_aac.m4a
ffmpeg -f lavfi -i sine=frequency=261.63:duration=2:sample_rate=22050 -ac 1 -c:a pcm_s16le mono_22050.wav
ffmpeg -f lavfi -i sine=frequency=329.63:duration=2:sample_rate=96000 -ac 2 -c:a flac stereo_96000.flac
ffmpeg -f lavfi -i sine=frequency=392:duration=2:sample_rate=48000 -ac 6 -channel_layout 5.1 -c:a pcm_s16le surround_48000.wav
ffmpeg -f lavfi -i sine=frequency=440:duration=30:sample_rate=44100 -ac 2 -c:a pcm_s16le long_pcm_s16le.wav
```

## Locked Outputs

For FFmpeg `n7.1.4-39-ga5faeca88f-20260611`:

| File | Size | SHA-256 |
|---|---:|---|
| `tone_pcm_s16le.wav` | 352,878 | `101b74ef127db194060ca1240e16551f92d3449d75f0248c3894963290f53de1` |
| `tone.flac` | 33,563 | `0239214dc0f0a21376185d71a482f4f0b1b704dc315a5432d2acb2d23783ce46` |
| `tone.mp3` | 49,571 | `a111e9605c587b55f99c43998d86220ea234c15c12b6b31ed29b015a8a744a42` |
| `tone_vbr.mp3` | 12,685 | `210201592638c074385c54797e8d35564327a06a058a5f28eaa4e57ed5f84751` |
| `tone_aac.m4a` | 41,557 | `32133a19bf3f4f5ced41dc2593511ccd48d97f4ca83aeaa23586f8ae68fc350e` |
| `mono_22050.wav` | 88,278 | `e4075edc6baa7c6f4e841dd99aa00e31be908472afaf8619f5c0eac5ed843f6d` |
| `stereo_96000.flac` | 59,178 | `fc620ee67a7f3c6b090f342e242150a52b648d6633c4dff3ef40f65e953f86e6` |
| `surround_48000.wav` | 1,152,102 | `5070a3a3f9e45a1d49311ef44f568c0dbd156d4b428fd7ea2c160dcf7cc588b0` |
| `long_pcm_s16le.wav` | 5,292,078 | `e9deb80bb404f66baa5ac400886ba56d433e65b57e350f64d2d2aa1b16054a57` |

Hash drift means the generator revision, command, codec configuration, or
container metadata changed and must be reviewed before the baseline is updated.
