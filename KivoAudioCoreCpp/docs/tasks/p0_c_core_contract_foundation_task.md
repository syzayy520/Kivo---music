# P0-C Core Contract Foundation — Task Specification

**Task ID:** KIVO-CPP-AUDIO-CORE-P0-C-CONTRACT-FOUNDATION-001  
**Phase:** P0-C  
**Type:** implementation / contract-definition / foundation  
**Execution Basis:** V10.1 FINAL P0-B READY LOCKED PATCHED + Enhancement Input Document  
**Prerequisite:** P0-B GO_TO_P0_C_TASK_AUTHORING  
**Date:** 2026-06-10  
**Status:** DRAFT  

---

## 1. Task Objective

Define and implement the playback semantics foundation for KivoAudioCoreCpp. P0-C establishes the core contract language that all subsequent phases (P0-D through P0-P) will speak. This includes:

- 14 core contract families
- Type definitions (enums, structs, traits/interfaces)
- Behavioral contracts and constraints
- Platform-neutral abstractions
- Comprehensive test coverage

**P0-C does NOT:**
- Implement real audio playback
- Connect WASAPI, FFmpeg, or mpv
- Create fake backends or test harnesses
- Modify P0-B output files
- Expand P0-B scope

---

## 2. Scope Definition

### 2.1 Allowed

| Category | Allowed Content |
|----------|-----------------|
| **Source files** | `src/core/contract/` only |
| **Test files** | `tests/contracts/` only |
| **Header files** | `include/kivo/core/contract/` only |
| **CMake targets** | Contract library target, contract test target |
| **Dependencies** | Standard C++20 library only |
| **Documentation** | Inline documentation in headers |

### 2.2 Forbidden

| Category | Forbidden Content |
|----------|-------------------|
| **Runtime** | No audio playback, no threads, no I/O |
| **Platform** | No WASAPI, no Windows API, no COM |
| **Decode** | No FFmpeg, no codec implementation |
| **Fake backends** | No fake implementations, no mocks (P0-D scope) |
| **State machine** | No state machine implementation (P0-J scope) |
| **Host integration** | No Tauri, no C ABI, no host callbacks |
| **External dependencies** | No third-party libraries |

---

## 3. Input Dependencies

### 3.1 P0-B Output

- CMake configure-only smoke
- Policy pack (7 policies)
- Gate scripts (7 gates)
- Project structure skeleton

### 3.2 Enhancement Input Document

**File:** `docs/architecture/p0c_through_p0i_enhancement_input.md`

**Relevant sections:**
- Section 1.1: Audio Time Model / Clock Domain Contract
- Section 1.2: Seek / Flush / Drain Contract
- Section 1.3: Audio Format Negotiation / Format Truth Contract
- Section 1.4: Output Policy / Bit-perfect Truth Contract
- Section 1.5: Realtime Thread / Ownership / Buffer Policy
- Section 1.6: Queue / Buffer Ownership + Generation Contract
- Section 1.7: Source / IO Reader / Container Boundary
- Section 1.8: CUE / Whole-File / Album Continuity Reservation
- Section 1.9: Error Taxonomy / Recovery Matrix
- Section 1.10: Playback State Machine Semantics
- Section 1.11: Command Ordering / Cancellation / User Race Contract
- Section 1.12: Observability / Telemetry / Ring Trace Contract
- Section 1.13: Privacy / Security / Credential Boundary
- Section 1.14: License / Distribution / Binary Provenance Policy

---

## 4. Contract Family Specifications

### 4.1 Audio Time Model / Clock Domain Contract

**Location:** `src/core/contract/clock/`

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
- Clock is an independent core contract, not attached to buffer or writer
- No Windows-specific naming in core clock types
- Windows `IAudioClock` only appears in P0-F WASAPI platform adapter
- Core only accepts platform-neutral clock snapshots

**Test requirements:**
- Unit tests for all clock types
- Behavior tests for clock reset rules
- Generation counter increment tests
- Drift estimation tests

### 4.2 Seek / Flush / Drain Contract

**Location:** `src/core/contract/seek/`

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
- Seek/Flush must not wait until P0-J state machine hardening
- Seek/device reset must cause old-generation frames to naturally expire
- Flush must have explicit scope, not "clear everything" brute semantics

**Test requirements:**
- Seek intent validation tests
- Flush scope tests
- Generation counter tests
- Stale frame discard tests
- State-specific seek behavior tests

### 4.3 Audio Format Negotiation / Format Truth Contract

**Location:** `src/core/contract/format/`

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
- FFmpeg `AVSampleFormat` / `AVFrame` / `AVPacket` must NOT enter core contract
- WASAPI `WAVEFORMATEX` / `WAVEFORMATEXTENSIBLE` must NOT enter core contract
- Core uses only platform-neutral format descriptors
- Shared / exclusive / bit-perfect / resample decisions must be explainable
- Device format mismatch must NOT silently truncate
- Channel remix must NOT silently happen

**Test requirements:**
- Format descriptor construction tests
- Format comparison tests
- Conversion policy tests
- Bit-perfect eligibility tests
- Platform neutrality verification tests

### 4.4 Output Policy / Bit-perfect Truth Contract

**Location:** `src/core/contract/output/`

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

**Test requirements:**
- State transition tests
- Bit-perfect eligibility tests
- Truth matrix tests
- Explanation generation tests

### 4.5 Realtime Thread / Ownership / Buffer Policy

**Location:** `src/core/contract/realtime/`

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
- No Windows API names in core contract types
- MMCSS / AVRT / COM apartment / Windows thread priority belong to P0-F/H platform policy
- Memory locking, page disable, PowerSetRequest are future evaluation items only, not P0-C hard requirements

**Test requirements:**
- Ownership transfer tests
- SPSC semantics tests
- Bounded transport tests
- Policy violation detection tests

### 4.6 Queue / Buffer Ownership + Generation Contract

**Location:** `src/core/contract/queue/`

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
- Seek / device reset / source switch must cause old-generation decoded frames to naturally expire
- Queue is NOT a UI queue
- Playback queue / decode queue / render queue must be distinct

**Test requirements:**
- Ownership transfer tests
- Generation counter tests
- Stale data discard tests
- Capacity management tests

### 4.7 Source / IO Reader / Container Boundary

**Location:** `src/core/contract/source/`

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
- AudioCore does NOT directly depend on NAS/WebDAV/cloud drive APIs
- AudioCore does NOT handle accounts, cookies, token refresh
- AudioCore does NOT handle media library indexing, cover art, lyrics, tags, UI metadata
- Source / Container / StreamProvider is a boundary, not a media library

**Test requirements:**
- Capability flag tests
- Error domain tests
- Interface definition tests
- Reservation verification tests

### 4.8 CUE / Whole-File / Album Continuity Reservation

**Location:** `src/core/contract/cue/`

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
- CUE / whole-file FLAC / APE splitting must NOT pollute core playback
- They must enter through source/container/track_segment boundary
- P0-L implements gapless, but P0-C/P0-G must reserve semantics now

**Test requirements:**
- Type definition tests
- Reservation verification tests
- Boundary tests

### 4.9 Error Taxonomy / Recovery Matrix

**Location:** `src/core/contract/error/`

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

**Test requirements:**
- Error domain tests
- Attribute tests
- Recovery action tests
- Error mapping tests

### 4.10 Playback State Machine Semantics

**Location:** `src/core/contract/state/`

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

**Test requirements:**
- State transition tests
- Illegal transition tests
- Edge case tests
- Recovery path tests

### 4.11 Command Ordering / Cancellation / User Race Contract

**Location:** `src/core/contract/command/`

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
- Commands must have generation / cancellation semantics
- Old commands must NOT pollute new playback sessions
- Host/UI must NOT directly manipulate render queue

**Test requirements:**
- Command ordering tests
- Cancellation tests
- Race condition tests
- Generation tests

### 4.12 Observability / Telemetry / Ring Trace Contract

**Location:** `src/core/contract/telemetry/`

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
- Realtime thread must NOT write logs synchronously
- Only lightweight in-memory aggregation or ring trace
- Crash report / telemetry must NOT leak full user paths, song names, or account tokens

**Test requirements:**
- Metric definition tests
- Ring trace event tests
- Constraint verification tests
- Privacy compliance tests

### 4.13 Privacy / Security / Credential Boundary

**Location:** `src/core/contract/privacy/`

**Reserved rules:**
- Tokens must NOT appear in logs
- WebDAV / cloud drive accounts must NOT enter AudioCore
- Crash reports must NOT contain full file paths
- Source IDs must be sanitizable
- Users can disable telemetry
- Remote caches must be clearable
- Credentials are managed by Host / Source Adapter
- AudioCore only receives opaque source handles / capability snapshots

**Test requirements:**
- Token leakage tests
- Path sanitization tests
- Credential boundary tests
- Telemetry opt-out tests

### 4.14 License / Distribution / Binary Provenance Policy

**Location:** `src/core/contract/license/`

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
- P0-C does NOT connect FFmpeg binaries
- P0-B/P0-C do NOT connect mpv binaries
- Any fallback runtime must have separate legal audit

**Test requirements:**
- License type tests
- Provenance tracking tests
- Policy enforcement tests

---

## 5. Implementation Structure

### 5.1 Directory Layout

```
KivoAudioCoreCpp/
├── include/
│   └── kivo/
│       └── core/
│           └── contract/
│               ├── clock/
│               ├── seek/
│               ├── format/
│               ├── output/
│               ├── realtime/
│               ├── queue/
│               ├── source/
│               ├── cue/
│               ├── error/
│               ├── state/
│               ├── command/
│               ├── telemetry/
│               ├── privacy/
│               └── license/
├── src/
│   └── core/
│       └── contract/
│           ├── clock/
│           ├── seek/
│           ├── format/
│           ├── output/
│           ├── realtime/
│           ├── queue/
│           ├── source/
│           ├── cue/
│           ├── error/
│           ├── state/
│           ├── command/
│           ├── telemetry/
│           ├── privacy/
│           └── license/
└── tests/
    └── contracts/
        ├── clock_tests/
        ├── seek_tests/
        ├── format_tests/
        ├── output_tests/
        ├── realtime_tests/
        ├── queue_tests/
        ├── source_tests/
        ├── cue_tests/
        ├── error_tests/
        ├── state_tests/
        ├── command_tests/
        ├── telemetry_tests/
        ├── privacy_tests/
        └── license_tests/
```

### 5.2 CMake Targets

```cmake
# Contract library (header-only or compiled)
add_library(kivo_core_contract INTERFACE)
target_include_directories(kivo_core_contract INTERFACE include/)

# Contract tests
add_executable(kivo_contract_tests tests/contracts/*.cpp)
target_link_libraries(kivo_contract_tests PRIVATE kivo_core_contract)
add_test(NAME contract_tests COMMAND kivo_contract_tests)
```

---

## 6. Acceptance Criteria

### 6.1 Functional Requirements

- [ ] All 14 contract families defined
- [ ] All required types implemented as C++20 types (enums, structs, classes)
- [ ] All behavioral contracts documented and tested
- [ ] All constraints verified by tests
- [ ] Platform neutrality maintained (no Windows/FFmpeg/mpv types)

### 6.2 Quality Requirements

- [ ] 100% test coverage for contract types
- [ ] All tests pass
- [ ] No compiler warnings
- [ ] Code follows .clang-format style
- [ ] Documentation complete for all public APIs

### 6.3 Scope Requirements

- [ ] No runtime implementation
- [ ] No platform-specific code
- [ ] No external dependencies
- [ ] No modification to P0-B output
- [ ] No scope expansion beyond contract definitions

---

## 7. Risk Assessment

### 7.1 Technical Risks

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Type design complexity | Medium | High | Iterative refinement, peer review |
| Test coverage gaps | Low | Medium | Comprehensive test planning |
| Platform neutrality violation | Low | High | Automated checks, code review |
| Scope creep | Medium | High | Strict adherence to allowed list |

### 7.2 Schedule Risks

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| Underestimated complexity | Medium | Medium | Buffer time in schedule |
| Dependency on P0-B | Low | High | P0-B already complete |
| Resource availability | Low | Medium | Clear task breakdown |

---

## 8. Testing Strategy

### 8.1 Unit Tests

- Type construction tests
- Type comparison tests
- Type conversion tests
- Boundary value tests
- Error condition tests

### 8.2 Behavioral Contract Tests

- State transition tests
- Ownership transfer tests
- Generation counter tests
- Constraint verification tests
- Edge case tests

### 8.3 Integration Tests

- Cross-contract interaction tests
- Contract compatibility tests
- API surface tests

---

## 9. Documentation Requirements

### 9.1 Inline Documentation

- All public types documented
- All public methods documented
- All constraints documented
- All examples provided

### 9.2 API Reference

- Generated from inline documentation
- Searchable
- Cross-referenced

### 9.3 Usage Guide

- Getting started guide
- Common patterns
- Best practices
- Anti-patterns

---

## 10. Delivery Schedule

### 10.1 Milestones

| Milestone | Description | Target Date |
|-----------|-------------|-------------|
| M1 | Contract structure setup | TBD |
| M2 | Clock domain contract | TBD |
| M3 | Seek/flush/drain contract | TBD |
| M4 | Format negotiation contract | TBD |
| M5 | Output policy contract | TBD |
| M6 | Realtime/thread contract | TBD |
| M7 | Queue/buffer contract | TBD |
| M8 | Source/IO contract | TBD |
| M9 | CUE/album reservation | TBD |
| M10 | Error taxonomy | TBD |
| M11 | State machine semantics | TBD |
| M12 | Command ordering | TBD |
| M13 | Telemetry contract | TBD |
| M14 | Privacy/security contract | TBD |
| M15 | License/provenance contract | TBD |
| M16 | Integration testing | TBD |
| M17 | Documentation completion | TBD |
| M18 | Final validation | TBD |

### 10.2 Deliverables

- [ ] Contract library (header-only or compiled)
- [ ] Contract test suite
- [ ] API documentation
- [ ] Usage guide
- [ ] Test coverage report
- [ ] Validation report

---

## 11. Validation Criteria

### 11.1 Gate Validation

All P0-B gates must continue to pass:
- Forbidden Token Gate: PASS
- Genealogy Gate: PASS
- No Empty Forest Gate: PASS
- No Audio Runtime Gate: PASS
- Policy Substance Gate: PASS
- Dependency/License Gate: PASS
- Toolchain/Configure Gate: PASS

### 11.2 Contract Validation

- All contract types compile
- All tests pass
- No platform-specific code
- No external dependencies
- No runtime implementation

### 11.3 Scope Validation

- Only `src/core/contract/`, `tests/contracts/`, `include/kivo/core/contract/` modified
- No modification to P0-B output
- No scope expansion

---

## 12. Go / No-Go Criteria

### 12.1 Go Criteria

- [ ] All 14 contract families defined
- [ ] All tests pass
- [ ] All gates pass
- [ ] Scope validation passes
- [ ] Documentation complete
- [ ] No platform-specific code
- [ ] No external dependencies

### 12.2 No-Go Criteria

- Any contract family missing
- Any test failure
- Any gate failure
- Scope violation
- Platform-specific code detected
- External dependency added
- Runtime implementation present

---

## 13. Post-P0-C Handoff

### 13.1 P0-D Dependencies

P0-D (Fake Backend Contract Proof) depends on:
- All 14 contract families from P0-C
- Test infrastructure from P0-C
- Contract interfaces from P0-C

### 13.2 P0-E Dependencies

P0-E (Render Boundary Contract) depends on:
- Clock domain contract
- Seek/flush/drain contract
- Format negotiation contract
- Queue/buffer ownership contract

### 13.3 Documentation Handoff

- Contract specification document
- API reference
- Usage guide
- Test coverage report

---

## 14. Appendix

### 14.1 References

- V10.1 FINAL P0-B READY LOCKED PATCHED
- P0-C through P0-I Enhancement Input Document
- KivoAudioCoreCpp Policy Pack

### 14.2 Glossary

- **Contract**: A formal specification of types, behaviors, and constraints
- **Platform-neutral**: No dependency on specific operating system or library
- **Generation counter**: A monotonic counter incremented on state changes
- **Stale data**: Data from a previous generation that should be discarded

### 14.3 Change History

| Date | Version | Description |
|------|---------|-------------|
| 2026-06-10 | 1.0 | Initial task specification |

---

**Task Status:** DRAFT  
**Next Action:** Review and approval  
**Responsible:** Project owner  
**Approval Required:** Yes