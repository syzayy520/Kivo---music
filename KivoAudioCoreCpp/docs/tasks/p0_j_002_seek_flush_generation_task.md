# P0-J-002 Seek, Flush, and Generation Execution

**Task:** KIVO-CPP-AUDIO-CORE-P0-J-002-SEEK-FLUSH-GENERATION
**Status:** COMPLETED
**Execution Basis:** `docs/architecture/audio_core_execution_roadmap.md`
**Last Updated:** 2026-06-12

## Purpose

Implement real local-file seek and flush behavior with generation advancement,
sample trimming, rapid-seek coalescing, and pause-state restoration.

## Natural Family Tree

```text
PlaybackSessionController
  -> serialized Seek command
  -> latest-target coalescing
  -> seek generation advance
  -> FfmpegDemuxSession::seek_to
  -> FfmpegAudioDecoder::flush
  -> FfmpegAudioConverter reset
  -> first-frame timestamp resolution
  -> pre-target sample trimming
  -> DecodedAudioBlockView with new decode generation
```

## Rules

- Seek target is expressed on the requested output sample timeline.
- Demux seek uses the selected audio stream time base.
- Decoder and converter buffered state are discarded after seek.
- The first returned block starts exactly at the requested output frame.
- Decode generation must strictly advance.
- A stale seek cannot mutate the active decoder.
- Repeated pending seeks coalesce to the latest target.
- Successful completion restores Playing, Paused, or Ready state.
- A seek initiated while paused remains paused and updates frozen position.
- Flush generation advances independently from seek generation.

## Exit Criteria

- WAV, FLAC, and MP3 seek tests pass at 48,000 output frames.
- First block offset equals the target.
- Decode generation is preserved on post-seek blocks.
- Stale seek is rejected.
- Rapid seek coalescing and paused seek restoration tests pass.
