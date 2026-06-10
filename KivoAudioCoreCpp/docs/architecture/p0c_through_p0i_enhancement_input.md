# P0-C through P0-I Contract Foundation Enhancement Input

**Task:** KIVO-CPP-AUDIO-CORE-POST-P0B-P0C-P0I-CONTRACT-FOUNDATION-ENHANCEMENT-INPUT-002  
**Type:** planning-only / post-p0b-review / next-phase-task-authoring-input  
**Current Phase:** Post-P0B Enhancement Input Planning  
**Execution Basis:** V10.1 FINAL P0-B READY LOCKED PATCHED  
**Prerequisite:** P0-B GO_TO_P0_C_READY_FOR_TASK_AUTHORING  
**Allowed:** docs/architecture/ only (planning artifact)  
**Last Updated:** 2026-06-10  

---

## 0. Purpose

This document consolidates the enhanced contract requirements for P0-C through P0-I. It is the formal task authoring input for P0-C task creation. It does NOT implement any contracts, does NOT create source files, does NOT modify P0-B output, and does NOT expand P0-B scope.

**This document is a planning artifact only. It lives in docs/architecture/ as a future phase rule reference.**

---

## 1. P0-C Core Contract Foundation — Enhanced Requirements

P0-C must define the playback semantics foundation. It is not just enums and headers — it is the language that every subsequent phase speaks.

### 1.1 Audio Time Model / Clock Domain Contract

**Required types:**
- `device_clock` — hardware sample clock abstraction
- `stream_clock` — decoded stream logical clock
- `timeline_clock` — user-facing playback position
- `decoded_position` — last decoded sample position
- `rendered_position` — last submitted to device position
- `sample_position` — absolute sample counter (64-bit)
- `frame_count` — frame count type (64-bit)
- `clock_generation` — generation counter for clock reset tracking
- `drift_estimate` — estimated drift between device and stream clocks
- `pause_resume_freeze_policy` — what happens to clocks during pause/resume
- `device_lost_position_report_rule` — how to report position after device lost
- `seek_clock_reset_rule` — how clocks reset after seek
- `drain_eos_timeline_rule` — timeline behavior at end-of-stream
- `gapless_timeline_continuity_rule` — timeline continuity across tracks (P0-L prep)

**Constraints:**
- Clock is an independent core contract, not attached to buffer or writer.
- No Windows-specific naming in core clock types.
- Windows `IAudioClock` only appears in P0-F WASAPI platform adapter.
- Core only accepts platform-neutral clock snapshots.

### 1.2 Seek / Flush / Drain Contract

**Required types:**
- `seek_intent` — user's seek request
- `seek_target` — target position (sample-accurate)
- `seek_accuracy` — exact / nearest keyframe / nearest sample
- `seek_generation` — generation counter incremented on seek
- `flush_scope` — what gets flushed
- `decoder_flush` — flush decoder internal state
- `queue_flush` — flush playback queue
- `renderer_flush` — flush device buffer
- `drain_semantics` — how to drain remaining audio
- `reset_generation` — generation reset on device/source change
- `stale_frame_discard` — rules for discarding old-generation frames
- `post_seek_clock_reset` — clock state after seek completes
- `seek_during_playing_state` — behavior definition
- `seek_during_paused_state` — behavior definition
- `seek_during_recovery_state` — behavior definition
- `seek_unsupported_explicit_state` — explicit error for unsupported seek
- `seek_anti_glitch_policy` — mute/fade during seek to prevent pops

**Constraints:**
- Seek/Flush must not wait until P0-J state machine hardening.
- Seek/device reset must cause old-generation frames to naturally expire.
- Flush must have explicit scope, not "clear everything" brute semantics.

### 1.3 Audio Format Negotiation / Format Truth Contract

**Required types:**
- `NativeDecodedFormat` — format as decoded by codec
- `CoreCanonicalFormat` — core's internal working format
- `RenderFormat` — format submitted to render boundary
- `DeviceFormat` — format accepted by audio device
- `AudioFormatDescriptor` — comprehensive format description
- `SampleFormat` — valid_bits, container_bits, endianness, signedness, alignment, planar/interleaved
- `ChannelLayout` — named layout + channel count + channel mask
- `FrameLayout` — interleaved vs planar
- `ChannelMaskEquivalent` — platform-neutral channel mask
- `FormatNegotiationResult` — outcome of format negotiation
- `ConversionPolicy` — what conversions are needed
- `ResampleDecision` — whether to resample and why
- `BitPerfectEligibility` — whether bit-perfect is possible
- `OutputPolicy` — shared/exclusive/bit-perfect decision

**Constraints:**
- FFmpeg `AVSampleFormat` / `AVFrame` / `AVPacket` must NOT enter core contract.
- WASAPI `WAVEFORMATEX` / `WAVEFORMATEXTENSIBLE` must NOT enter core contract.
- Core uses only platform-neutral format descriptors.
- Shared / exclusive / bit-perfect / resample decisions must be explainable.
- Device format mismatch must NOT silently truncate.
- Channel remix must NOT silently happen.

### 1.4 Output Policy / Bit-perfect Truth Contract

**Required states:**
- `Shared` — using Windows shared mode
- `Exclusive` — using WASAPI exclusive mode
- `BitPerfectRequested` — user requested bit-perfect
- `BitPerfectActive` — bit-perfect confirmed active
- `BitPerfectRejected` — bit-perfect requested but rejected (with reason)
- `BitPerfectUnavailable` — device does not support bit-perfect
- `ResampleRequired` — resampling needed
- `ResampleForbidden` — resampling disabled by policy
- `ResampleNotNeeded` — formats match, no resample
- `ReplayGainEnabled` — ReplayGain processing active
- `VolumeEnabled` — volume adjustment active
- `DspEnabled` — DSP processing active
- `ChannelRemixEnabled` — channel remix active
- `WindowsAudioEngineParticipates` — Windows audio engine in path
- `DeviceFormatEqualsSourceFormat` — format match confirmation
- `SampleTransparentPath` — no sample format conversion

**Bit-perfect must answer:**
- Is shared mode enabled?
- Is exclusive mode enabled?
- Does audio pass through Windows audio engine?
- Does audio pass through resampler?
- Does audio pass through ReplayGain?
- Does audio pass through volume?
- Does audio pass through DSP?
- Is int/float conversion happening?
- Is channel remix happening?
- What is the final device format?

**Constraints:**
- Bit-perfect is NOT a UI slogan. It is a provable state.
- mpv fallback does NOT participate in bit-perfect truth claims.

### 1.5 Realtime Thread / Ownership / Buffer Policy

**Required rules:**
- Realtime path prohibits heap allocation
- Realtime path prohibits blocking I/O
- Realtime path prohibits heavy locks
- Realtime path prohibits synchronous logging
- Realtime path prohibits host callbacks
- Realtime path prohibits complex state machine decisions
- Realtime path prohibits exceptions crossing boundary
- Buffers must be pre-allocated or have explicit ownership
- Transport must be bounded
- SPSC (single-producer single-consumer) semantics must be explicit

**Constraints:**
- No Windows API names in core contract types.
- MMCSS / AVRT / COM apartment / Windows thread priority belong to P0-F/H platform policy.
- Memory locking, page disable, PowerSetRequest are future evaluation items only, not P0-C hard requirements.

### 1.6 Queue / Buffer Ownership + Generation Contract

**Required types:**
- `audio_frame_block_owner` — who owns the buffer block
- `block_lifetime` — when the block can be freed
- `movable_rule` / `non_movable_rule` — buffer movement semantics
- `decoder_memory_reference_policy` — can decoder hold references across boundary
- `cross_thread_share_rule` — how blocks cross thread boundaries
- `queue_capacity_unit` — frames / bytes / duration
- `capacity_by_frames` — frame-based capacity
- `capacity_by_bytes` — byte-based capacity
- `capacity_by_duration` — duration-based capacity
- `seek_generation` — generation counter for seek
- `stream_generation` — generation counter for stream
- `device_generation` — generation counter for device
- `flush_generation` — generation counter for flush
- `stale_generation_discard` — automatic discard of old-generation data
- `partial_write_ownership` — who owns unwritten portion
- `delayed_write_ownership` — who owns delayed submission

**Constraints:**
- Seek / device reset / source switch must cause old-generation decoded frames to naturally expire.
- Queue is NOT a UI queue.
- Playback queue / decode queue / render queue must be distinct.

### 1.7 Source / IO Reader / Container Boundary

**Required types (definition only, no implementation):**
- `source_contract` — abstract source interface
- `stream_provider` — provides byte streams
- `byte_range_source` — supports range requests
- `packet_source` — provides decoded packets
- `source_capability` — what the source can do
- `source_error_domain` — source-specific errors
- `read_ahead_policy` — how much to read ahead
- `cancellable_read` — read that can be cancelled
- `time_range` — time span in source
- `track_segment` — a segment of a track
- `virtual_track` — a logical track (may span segments)
- `cache_state` — cache status

**Source capability flags:**
- `seekable`, `range_readable`, `length_known`, `duration_known`
- `read_ahead_supported`, `cancel_supported`
- `temporary_failure`, `fatal_failure`, `network_disconnect`
- `permission_denied`, `credential_required`
- `cache_available`, `cache_partial`, `cache_stale`

**Future source types (reserved, not implemented):**
- `LocalFileSource`
- `SMB_NAS_Source`
- `WebDAV_Source`
- `CloudDriveSource`
- `CachedRemoteSource`

**Constraints:**
- AudioCore does NOT directly depend on NAS/WebDAV/cloud drive APIs.
- AudioCore does NOT handle accounts, cookies, token refresh.
- AudioCore does NOT handle media library indexing, cover art, lyrics, tags, UI metadata.
- Source / Container / StreamProvider is a boundary, not a media library.

### 1.8 CUE / Whole-File / Album Continuity Reservation

**Reserved types (definition only):**
- `cue_sheet_boundary` — CUE sheet track boundary
- `whole_album_file` — single-file album (FLAC/APE)
- `virtual_track_segment` — a virtual track within a larger file
- `source_time_range` — time range in source file
- `timeline_range` — time range in playback timeline
- `gapless_metadata` — metadata needed for gapless playback
- `codec_delay` — decoder delay in samples
- `padding` — encoder padding in samples
- `preroll` — samples needed before decode start
- `prefetch` — samples to prefetch for gapless
- `cross_track_clock_continuity` — clock continuity across tracks

**Constraints:**
- CUE / whole-file FLAC / APE splitting must NOT pollute core playback.
- They must enter through source/container/track_segment boundary.
- P0-L implements gapless, but P0-C/P0-G must reserve semantics now.

### 1.9 Error Taxonomy / Recovery Matrix

**Error domains:**
- `UserError`, `MediaError`, `DecodeError`, `FormatError`
- `DeviceError`, `BackendError`, `HostError`, `InternalBug`
- `DeviceLost`, `FormatChanged`, `DecoderStarved`
- `BufferUnderrun`, `BufferOverrun`
- `UnsupportedFormat`, `SourceUnavailable`
- `NetworkInterrupted`, `PermissionDenied`, `CacheInvalid`

**Per-error attributes:**
- `severity` — fatal / recoverable / warning / info
- `recoverable` — can system continue
- `retryable` — worth retrying
- `requires_reopen_device` — device must be reopened
- `requires_rebuild_decoder` — decoder must be recreated
- `requires_user_action` — user must intervene
- `safe_to_continue` — safe to continue playback
- `recovery_action` — recommended recovery
- `host_notification` — must notify host
- `metrics_event` — must record metric
- `safe_cleanup_path` — cleanup sequence

**Recovery actions:**
- `Retry`, `Drain`, `Flush`, `ReopenDevice`, `ReconfigureFormat`
- `SkipFrame`, `SkipTrack`, `StopPlayback`, `ReportToUser`
- `RebuildSource`, `RebuildDecoder`, `RebuildRenderer`

### 1.10 Playback State Machine Semantics

**Required states:**
- `Idle`, `Opening`, `Ready`, `Playing`, `Pausing`, `Paused`
- `Seeking`, `Draining`, `Recovering`, `Stopped`, `Failed`, `Closed`

**Illegal transitions (examples):**
- `Playing -> Opening` — FORBIDDEN
- `Seeking -> Seeking` — merge or queue (must define)
- `Recovering -> Seek` — reject or queue (must define)
- `Draining -> Pause` — must define behavior
- `Failed -> Close` — must succeed
- `Closed -> any write` — must fail
- Device lost during seek — must define
- Host shutdown during drain — must define

### 1.11 Command Ordering / Cancellation / User Race Contract

**Scenarios to handle:**
- Continuous seek (rapid seeking)
- Next track spam (rapid track switching)
- Pause while switching track
- Seek while recovering
- Close while draining
- Shutdown while writing
- Library scan while playing
- Source disconnect while seeking
- Device lost while flushing

**Constraints:**
- Commands must have generation / cancellation semantics.
- Old commands must NOT pollute new playback sessions.
- Host/UI must NOT directly manipulate render queue.

### 1.12 Observability / Telemetry / Ring Trace Contract

**Metrics (definition only):**
- `frames_decoded`, `frames_queued`, `frames_written`, `frames_consumed_estimated`
- `buffer_fill_frames`, `buffer_low_watermark`
- `underrun_count`, `overrun_count`
- `device_latency_ms`, `decode_latency_ms`, `render_callback_interval_ms`
- `clock_drift_ppm`
- `seek_flush_generation`, `device_generation`, `stream_generation`
- `state_transition_count`
- `source_read_latency_ms`, `cache_hit_ratio`, `network_retry_count`
- `format_renegotiation_count`

**Ring trace events (definition only):**
- `OpenSource`, `OpenDevice`, `NegotiateFormat`, `StartStream`
- `DecoderPush`, `QueuePop`, `RenderWrite`
- `Underrun`, `Overrun`
- `SeekBegin`, `FlushDecoder`, `FlushQueue`, `FlushRenderer`, `SeekCommit`
- `Pause`, `Resume`, `Drain`
- `DeviceLost`, `SourceLost`
- `RecoverBegin`, `RecoverCommit`
- `FormatChanged`, `FallbackRequested`, `FallbackRejected`

**Constraints:**
- Realtime thread must NOT write logs synchronously.
- Only lightweight in-memory aggregation or ring trace.
- Crash report / telemetry must NOT leak full user paths, song names, or account tokens.

### 1.13 Privacy / Security / Credential Boundary

**Reserved rules:**
- Tokens must NOT appear in logs.
- WebDAV / cloud drive accounts must NOT enter AudioCore.
- Crash reports must NOT contain full file paths.
- Source IDs must be sanitizable.
- Users can disable telemetry.
- Remote caches must be clearable.
- Credentials are managed by Host / Source Adapter.
- AudioCore only receives opaque source handles / capability snapshots.

### 1.14 License / Distribution / Binary Provenance Policy

**Reserved items:**
- FFmpeg LGPL/GPL build strategy
- Dynamic link vs static link policy
- Nonfree codec policy
- mpv fallback license audit
- License notices
- SBOM (Software Bill of Materials)
- Binary provenance
- Configure flags record
- Third-party binary hash
- Installer notices
- Commercial claim matrix

**Constraints:**
- P0-C does NOT connect FFmpeg binaries.
- P0-B/P0-C do NOT connect mpv binaries.
- Any fallback runtime must have separate legal audit.

---

## 2. P0-D Fake Backend — Enhanced as Fault Injection System

P0-D is NOT just happy-path fake playback. It must be a fault injection system that proves the contract works under failure.

### Required fault injection capabilities:

- Manual clock advance
- Fake clock drift injection
- Slow consumer injection
- Delayed write
- Partial write
- Forced underrun
- Forced overrun
- Forced device lost
- Forced format change
- Forced source lost
- Forced network stall
- Forced cache miss
- Stale generation write
- Write after stop
- Write during seek
- Drain timeout
- Queue pressure injection
- Command cancellation injection

### Goal:

Without WASAPI / FFmpeg / mpv / NAS / WebDAV, prove that playback semantics, error recovery, generation discard, seek/flush, clock drift, format change, and source failure contracts work correctly.

---

## 3. P0-E Render Boundary — Failure-First

P0-E must NOT only test happy paths. Failure testing must come first.

### Required failure tests:

- Write after stop
- Write during seek
- Write stale generation
- Underrun
- Overrun
- Device lost signal
- Flush while rendering
- Drain timeout
- Reset during write
- Invalid format
- Unsupported format
- Boundary failure mapping
- Telemetry emission on failure
- No platform type leakage proof

---

## 4. P0-F WASAPI Adapter Shell — Reserved

P0-F does NOT implement full real playback, but the shell must reserve:

- Shared mode initialization path
- Exclusive mode initialization path
- Device capability query boundary
- Format support result boundary
- Format mismatch mapping
- Device lost / invalidated status
- Adapter lifecycle
- Buffer alignment handling policy
- Glitch / underrun telemetry event boundary
- Platform API quarantine proof
- Device identity
- `device_generation`
- Reopen policy

**Constraints:**
- WASAPI types must NOT enter core contract.
- Windows API names must NOT appear in core.
- Specific API usage confirmed against official docs at P0-F implementation time.
- MMCSS / AVRT / COM apartment only in platform policy.

---

## 5. P0-G Decode Contract + FFmpeg Seam — Layered

P0-G must NOT simplify FFmpeg into a single decoder. It must be layered:

### Required seams:

- Source seam (byte/stream provider)
- Container / demux seam
- Packet source
- Decode session
- Audio decoder seam
- Seekable source
- Media probe
- Metadata for gapless
- Codec delay / padding / preroll
- Decode generation
- Format conversion request

**Constraints:**
- Seek belongs to demux/container/source boundary, not just decoder.
- `AVFormatContext` / `AVCodecContext` / `AVPacket` / `AVFrame` must NOT enter core contract.
- Core only accepts platform-neutral `MediaInfo` / `DecodedAudioBlock` / `DecodeResult` / `SeekResult`.
- Network / NAS / WebDAV source not in P0-G implementation, but seam must support non-local sources.

---

## 6. P0-H / P0-I — Anti-Demo Requirements

### P0-H Minimum Real WASAPI Output must include:

- Device open / close
- Format negotiation visible
- Start / stop / reset
- Underrun metric
- Device invalidated mapping
- No core pollution
- No fake success
- Unsupported feature explicit
- Shared/exclusive path status explicit
- `device_generation` visible

### P0-I FFmpeg Decode -> WASAPI Output Loop must include:

- `decode_generation`
- `source_generation`
- Format conversion policy visible
- Seek disabled or explicitly unsupported
- Flush disabled or explicitly unsupported
- Device format mismatch must NOT silently truncate
- No `AVFrame` / `AVPacket` leaking into core
- No `WAVEFORMAT` leaking into core
- Source failure explicit
- Unsupported remote source explicit

---

## 7. Deferred Phases — Reservation Only

### P0-K Commercial Stability / Device Matrix

Must cover:
- Onboard sound cards, USB DACs, Bluetooth, HDMI Audio
- High sample rate files, corrupted files, very large FLAC, VBR MP3, CUE whole-file
- NAS slow I/O, WebDAV disconnect, removable drive plug/unplug
- Sleep/wake, device hot-plug, continuous 10-hour playback

### P0-L Gapless / Timeline Accuracy

Reserved: track boundary, timeline continuity, sample-accurate position, codec delay/padding, preroll/prefetch, cross-track clock continuity, virtual track segment.

### P0-M Exclusive / Bit-perfect Truth

Reserved: shared vs exclusive output policy, bit-perfect eligibility, no-DSP proof, no-volume proof, no-ReplayGain proof, no-resample proof, device-format-equals-source proof.

### P0-N ReplayGain / Volume / DSP / Resampler Boundary

Reserved: gain/volume application point, fixed vs float boundary, DSP enabled state, resampler bypass path, resample latency frames, tail handling, dither policy, channel remix policy, ReplayGain disabled under bit-perfect.

### P0-O Host ABI / Tauri Integration Boundary

Reserved: Host Source Resolver, local/NAS/SMB/WebDAV/cloud-drive resolvers, credential boundary, opaque source handle, playback plan, transition plan, queue snapshot, host command serialization.

### P0-P Commercial Release Hardening

Reserved: installer, signing, crash diagnostics, privacy redaction, license notices, SBOM, FFmpeg build provenance, mpv fallback audit, user opt-out telemetry.

---

## 8. mpv/libmpv Fallback Boundary

**Status:** Reference / benchmark / fallback research ONLY.

**Rules:**
- mpv must NOT enter KivoAudioCoreCpp core.
- mpv must NOT enter core contract.
- mpv must NOT pollute self-built clock / queue / buffer / state machine.
- mpv does NOT participate in bit-perfect / gapless / audio truth claims.
- mpv fallback is called "Compatibility Mode" or "Emergency Playback Mode".
- mpv fallback must have independent telemetry / error reporting.
- mpv fallback must have independent license / distribution audit.
- mpv fallback should live at Host layer or in independent process/adapter, not mixed with main core.

---

## 9. NAS / WebDAV / Cloud Drive Source Boundary

**Status:** Reserved for future phases. NOT implemented in P0-C through P0-I.

**Rules:**
- AudioCore does NOT directly depend on NAS/WebDAV/cloud drive APIs.
- AudioCore does NOT handle accounts, cookies, token refresh.
- AudioCore does NOT handle media library indexing, cover art, lyrics, tags, UI metadata.
- Source / Container / StreamProvider is a boundary, not a media library.
- Credentials are managed by Host / Source Adapter.
- AudioCore only receives opaque source handles / capability snapshots.

---

## 10. P0-B Scope Non-Expansion Confirmation

This document does NOT add to P0-B:

- No docs/specs/ directory
- No ADR file forest
- No tests/corpus/
- No src/, tests/, core/, platform/, decode/, playback/
- No fake backend implementation
- No real contract implementation
- No WASAPI, FFmpeg, mpv
- No NAS/WebDAV/cloud drive adapter
- No Crash Reporter SDK (Sentry / Crashpad / MiniDump)
- No PowerSetRequest hard policy
- No memory locking / page disable hard policy
- No unverified Windows API details

All content in this document lives in `docs/architecture/` as a future phase rule reference. It is a planning artifact, not an implementation artifact.

---

## 11. Platform API Verification Requirement

All Windows / FFmpeg / mpv specific API names mentioned in this document are conceptual references for planning purposes. At implementation time (P0-F, P0-G, P0-H), each API must be verified against:

- Microsoft official WASAPI documentation
- FFmpeg official API documentation
- mpv official API documentation
- Windows SDK header verification

No API name in this document is a hard implementation commitment. They are boundary descriptions.

---

## 12. Closeout Summary

**Task:** KIVO-CPP-AUDIO-CORE-POST-P0B-P0C-P0I-CONTRACT-FOUNDATION-ENHANCEMENT-INPUT-002  
**Type:** planning-only  
**Files Created:** 1 (this document)  
**Files Modified:** 0  
**Location:** KivoAudioCoreCpp/docs/architecture/ (within PROJECT_ROOT)  
**P0-B Scope Expansion:** NONE  
**Implementation:** NONE  

### Content Delivered:

1. P0-C enhanced input: 14 contract families with full type/responsibility/constraint specification
2. P0-D enhanced input: fault injection system (18 injection capabilities)
3. P0-E enhanced input: failure-first testing (14 failure scenarios)
4. P0-F reserved: WASAPI adapter shell (12 reservation items)
5. P0-G layered: decode contract + FFmpeg seam (11 seam layers)
6. P0-H/P0-I anti-demo: explicit requirements preventing demo-ware
7. P0-K through P0-P: deferred but reserved with specific coverage requirements
8. mpv fallback boundary: explicit rules and isolation requirements
9. NAS/WebDAV/cloud drive source boundary: explicit rules
10. P0-B non-expansion: confirmed, no scope creep
11. Platform API verification: required at implementation time

---

## 13. Go / No-Go

**Conclusion:** GO_TO_P0_C_TASK_AUTHORING

**Reasoning:**
1. P0-B completed with all gates PASS
2. CMake 4.3.3 + MSVC 19.51 + VS 2026 confirmed
3. This document is planning-only, no scope expansion
4. All enhancement inputs are documented as future phase rules
5. No implementation files created
6. No P0-B output modified
7. No platform API hard-coding (deferred to implementation time)

**Next step:** Project owner creates P0-C task(s) based on this enhancement input document.
