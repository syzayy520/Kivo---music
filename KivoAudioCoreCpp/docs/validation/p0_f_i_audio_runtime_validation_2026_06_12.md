# P0-F Through P0-J Audio Runtime Validation

**Date:** 2026-06-12
**Platform:** Windows x64
**Compiler:** MSVC 19.51 / Visual Studio 2026
**Generator:** Ninja
**Configuration:** Debug with `/W4 /WX`
**Result:** PASS

## Build and Static Structure

- Full configured build passed.
- Every public header compiled independently and in one combined translation
  unit.
- Public headers contain no FFmpeg, WASAPI, COM, or Windows SDK implementation
  types outside the authorized platform boundary.
- FFmpeg SDK is supplied only through external `KIVO_FFMPEG_ROOT`.
- No configure, build, test, or gate download mechanism exists.
- Repository gates: 9/9 passed.
- `git diff --check`: passed.

## CTest

All 10 registered groups passed:

1. Contract tests.
2. Capability tests.
3. Render boundary tests.
4. Render queue and RenderPump tests.
5. Decode boundary and local source tests.
6. Deterministic fake renderer tests.
7. WASAPI mapping, error, and worker tests.
8. FFmpeg WAV, FLAC, MP3, corruption, conversion, generation, and EOS tests.
9. Playback session lifecycle, seek, generation, and recovery tests.
10. Decode-to-render queue lookahead, chunking, generation, and backpressure
    tests.

## Real Generated PCM Output

The WASAPI shared event-driven tone path completed three repeated cycles:

- 48,000 Hz stereo.
- 144,000 total submitted and worker-accepted frames.
- MMCSS `Pro Audio` registration: true.
- MMCSS high priority: true.
- Wait timeouts: 0.
- Underruns: 0.
- Device invalidations: 0.

## Real Local-File Output

Each generated fixture completed the full path:

```text
LocalFileByteSource
  -> FFmpeg demux/decode
  -> libswresample conversion
  -> bounded SPSC queue
  -> RenderPump
  -> MMCSS render worker
  -> WASAPI shared endpoint
```

### WAV PCM S16LE

- Native: 44,100 Hz stereo signed 16-bit.
- Output: 48,000 Hz stereo Float32.
- Decoded frames: 96,000.
- Queued frames: 96,000.
- Worker-accepted frames: 96,000.
- Underruns / wait timeouts / invalidations: 0 / 0 / 0.

### FLAC

- Native: 44,100 Hz stereo signed 16-bit.
- Output: 48,000 Hz stereo Float32.
- Decoded frames: 96,000.
- Queued frames: 96,000.
- Worker-accepted frames: 96,000.
- Underruns / wait timeouts / invalidations: 0 / 0 / 0.

### MP3

- Native: 44,100 Hz stereo planar Float32.
- Output: 48,000 Hz stereo interleaved Float32.
- FFmpeg skip/discard sample metadata applied to playable duration truth.
- Decoded frames: 96,000.
- Queued frames: 96,000.
- Worker-accepted frames: 96,000.
- Underruns / wait timeouts / invalidations: 0 / 0 / 0.

## 60-Minute Real-Device Endurance

The silent stability path completed one uninterrupted 60-minute run through
the production WASAPI shared event-driven renderer:

- Sample rate: 48,000 Hz stereo.
- Endpoint buffer: 4,800 frames / 100 ms.
- Submitted frames: 172,800,000.
- Worker-accepted frames: 172,800,000.
- MMCSS `Pro Audio` registration: true.
- MMCSS high priority: true.
- Worker wait timeouts: 0.
- Underruns: 0.
- Device invalidations: 0.
- Standard error output: empty.

The process started at 2026-06-12 03:44:01 Asia/Shanghai and completed at
2026-06-12 04:44:01. This closes the P0-F/H-001 endurance criterion.
