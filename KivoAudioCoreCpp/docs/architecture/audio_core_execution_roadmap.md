# KivoAudioCoreCpp Execution Roadmap

**Document ID:** KIVO-CPP-AUDIO-CORE-EXECUTION-ROADMAP-001
**Status:** ACTIVE EXECUTION BASELINE
**Effective Date:** 2026-06-12
**Applies To:** `KivoAudioCoreCpp/`
**Architecture Basis:** V10.1 FINAL P0-B READY LOCKED PATCHED
**Current Execution Position:** P0-N processing boundary

---

## 1. Authority and Purpose

This document is the authoritative implementation sequence for
KivoAudioCoreCpp after the P0-B skeleton and the initial P0-C contract work.

V10.1 remains the architecture constitution. Its platform isolation,
realtime-safety, audio-truth, licensing, and project-root rules remain in
force. This roadmap does not replace those rules. It converts them into an
execution order based on the repository as it exists on 2026-06-12.

When older planning documents describe a different next step, use the
following precedence:

1. Safety, legal, privacy, and project-root constraints from V10.1.
2. This execution roadmap.
3. The current phase taskbook.
4. Older phase-planning and historical closeout documents.

No taskbook may silently change this sequence. A sequence change requires an
explicit roadmap amendment with a reason, impact analysis, and owner approval.

The primary objective is:

> Build a commercially reliable, independently controlled Windows audio core
> that progresses from provable contracts to deterministic simulation, real
> WASAPI output, FFmpeg decode, recovery, audio accuracy, and release quality.

### 1.1 Policy Applicability

**Purpose:** Establish the binding implementation order and completion evidence
for the remaining audio-core phases.

**Scope:** Architecture, contracts, validation, fake backends, platform output,
decode, product-core coordination, quality, host integration, and release work
inside `KivoAudioCoreCpp/`.

**Current Phase:** P0-N processing boundary and strict bypass proof.

**Allowed:** Work that directly satisfies the active milestone, its tests, its
validation requirements, or a documented correctness, safety, legal, privacy,
or architecture blocker.

**Forbidden:** Silent phase skipping, platform leakage into core, demo-only
success paths, unbounded planning expansion, hidden audio conversion, false
quality claims, and implementation outside the project root.

**Gate:** Each milestone must satisfy its listed exit criteria and the active
repository validation suite before the next milestone becomes implementation
work.

**Failure Classification:** A failed exit criterion is a source or architecture
failure. A missing local prerequisite is an environment blocker. Neither may
be reported as a pass.

**Future Phase Ownership:** Each later milestone owns the implementation areas
listed in this roadmap. Earlier milestones may reserve neutral semantics but
must not pre-implement later platform or product features.

**Deferred Items:** Hi-Res, DSD, DoP, and mpv compatibility implementation stay
deferred until the primary PCM path, stability evidence, and release controls
are mature.

**Last Updated:** 2026-06-12.

---

## 2. Non-Negotiable Architecture Rules

The following rules apply to every phase.

### 2.1 Core isolation

- Core contracts and product-core coordination must not expose WASAPI, COM,
  Windows SDK, FFmpeg, mpv, STL ABI, or host UI types.
- WASAPI details belong below a platform adapter boundary.
- FFmpeg details belong below a decode adapter boundary.
- Host and Tauri integration belong behind a versioned host ABI.
- mpv/libmpv remains reference, benchmark, or separately isolated
  compatibility research. It is not part of the primary audio core.

### 2.2 Realtime path

The realtime render path must not perform:

- Heap allocation.
- Blocking file, network, or device I/O beyond the required render API call.
- Decode, demux, or FFmpeg calls.
- Synchronous logging.
- Host or UI callbacks.
- Long or unbounded locking.
- Exception propagation across the realtime boundary.
- Complex policy or state-machine decisions.

Realtime transport must be bounded. Buffer ownership, lifetime, generation,
and partial-consumption behavior must be explicit.

### 2.3 Audio truth

- Sample format must distinguish valid bits from container bits.
- Frame size must be derived from container representation, not signal-valid
  bits alone.
- Channel layout must include semantic layout and a platform-neutral channel
  mask or equivalent.
- Format conversion, resampling, channel remix, gain, volume, and DSP must
  never happen silently.
- Bit-perfect must be reported as requested, eligible, active, rejected, or
  unknown, with an explanation.
- Shared mode must not be advertised as bit-perfect by default.
- Gapless and position reporting must use sample-domain semantics rather than
  imprecise floating-point duration as the source of truth.

### 2.4 Failure and observability

- Device loss, source loss, malformed media, underrun, overrun, unsupported
  format, stale generation, timeout, and cancellation are normal modeled
  outcomes, not exceptional afterthoughts.
- Realtime observations use bounded counters or an in-memory ring trace.
- User paths, account data, credentials, and tokens must not leak into logs,
  telemetry, diagnostics, or crash bundles.

### 2.5 Commercial and dependency control

- Dependency versions, build flags, binary provenance, hashes, and licenses
  must be reproducible and auditable.
- FFmpeg configuration and LGPL/GPL implications are release gates.
- Codec availability does not automatically authorize a commercial claim.
- Test media must have known licensing and checksums.

---

## 3. Execution Method

### 3.1 Work in vertical proofs

Each phase must produce an executable proof of its responsibility. A phase is
not complete because headers, enums, documents, or directories exist.

Examples:

- P0-C proves public-contract composition and semantics.
- P0-D/P0-E proves deterministic write and failure behavior.
- P0-H proves real PCM reaches a real WASAPI device.
- P0-I proves a real local file is decoded and rendered.
- P0-J proves race, cancellation, seek, and recovery behavior.

### 3.2 Planning limit

Planning is required before implementation, but planning must be proportional
to risk.

- One concise implementation task may cover one coherent behavioral slice.
- Formatting-only or initializer-only taskbooks should not interrupt a
  behavior milestone unless they fix a gate, correctness issue, or review
  finding.
- Do not create a new family, directory, or abstraction without an immediate
  consumer or test in the same milestone.
- Do not expand validation tooling after it can reliably run the required
  configure, build, test, and gate sequence.
- Prefer implementation evidence over additional roadmap prose.

### 3.3 Definition of done for every implementation slice

Every implementation slice must:

1. State the behavior being added.
2. State non-goals and ownership boundaries.
3. Add or update tests at the same behavioral level.
4. Build from a clean generated directory.
5. Pass the relevant validation entry.
6. Avoid new public-header collisions and hidden transitive includes.
7. Update documentation only where behavior or operation changed.
8. Leave the working tree clean after commit.

### 3.4 Validation command objective

The repository must converge on one supported command that:

1. Locates or initializes the supported MSVC environment.
2. Configures a clean build directory.
3. Builds all required targets.
4. Runs all tests.
5. Runs all active gates.
6. Classifies source failures separately from environment blockers.

The command must work from an ordinary PowerShell session on a correctly
installed development machine. Requiring the user to manually discover and
open a special shell is an interim condition, not the final developer
experience.

---

## 4. Milestone 0: Close P0-C Correctly

**Priority:** Immediate
**Purpose:** Make the contract layer composable and trustworthy before any
fake or real backend depends on it.

P0-C is not closed merely because isolated contract tests pass. Its public
headers must compose in realistic translation units and its audio model must
be internally consistent.

### 4.1 Required corrections

#### Public-header composition

- Resolve the duplicate `BufferLifetimeProof` definitions.
- Audit the complete namespace for duplicate public type names.
- Add a target that includes all public headers in one translation unit.
- Add independent-header compilation checks so each public header supplies
  its own direct dependencies.
- Add representative consumer composition tests across contract families.
- Keep include order from changing observable declarations.

#### Audio format truth

- Define valid bits and container bits without contradictory duplicate
  fields.
- Derive bytes per sample and bytes per frame from container representation.
- Reject inconsistent combinations such as an `Int16` encoding with a
  conflicting container description.
- Make planar versus interleaved representation explicit.
- Ensure channel mask and channel count agree.
- Make format equality strong enough for negotiation and bit-perfect truth.
- Add tests for 16-bit PCM, packed or containerized 24-bit PCM, 32-bit PCM,
  float PCM, mono, stereo, 5.1, 7.1, and invalid combinations.

#### Generic result correctness

- Include direct standard-library dependencies such as `<utility>` where used.
- Audit conditional `noexcept` behavior for generic types.
- Define or document value/error accessor preconditions.
- Prevent a default internal state from claiming an error without valid error
  information.

#### Build and repository truth

- Remove hard dependence on one exact Visual Studio installation path where
  practical.
- Make validation initialize the discovered MSVC environment.
- Exclude `.build/`, `build/`, `out/`, and CMake-generated files from source
  genealogy and runtime-carrier scans.
- Update README and policy status to describe the current phase truthfully.
- Keep validation output concise enough to diagnose failures.

### 4.2 Required tests and gates

- All-header composition target.
- One-header-per-translation-unit compile checks.
- Contract tests.
- Capability tests.
- Duplicate public symbol/name audit.
- Clean configure and build.
- Gate suite with generated-directory exclusion.
- `git diff --check`.

### 4.3 Exit criteria

P0-C closes only when:

- A clean checkout can run the supported validation path.
- All contract and capability tests pass.
- All active gates pass.
- All public headers compile independently.
- All public headers compile together.
- No duplicate public type definition remains.
- PCM frame-size and validity tests cover valid/container-bit differences.
- README, policy index, and phase status agree.

### 4.4 Stop conditions

Do not begin fake-backend implementation if:

- Public headers cannot compose.
- Audio format truth is contradictory.
- Validation passes only because tests isolate conflicting headers.
- The normal validation entry cannot distinguish environment blocking from
  source failure.

---

## 5. Milestone 1: P0-D/P0-E Deterministic Render Proof

**Order:** Minimal render boundary first, fake backend second
**Purpose:** Establish a stable behavior surface, then prove it under
deterministic failure injection.

P0-D and P0-E form one implementation milestone even if they retain separate
closeouts. Building a fake backend before defining the boundary it implements
would create a disposable private API.

### 5.1 Step 1: Minimal platform-neutral render boundary

Define the minimum concepts required for a renderer:

- Renderer identity and capability snapshot.
- Open, start, write, flush, drain, reset, stop, and close lifecycle.
- Render format request and accepted format result.
- Audio block view with frame count and frame offset.
- Stream, seek, flush, and device generations.
- Full, partial, delayed, rejected, and failed write outcomes.
- Explicit ownership of unwritten frames after partial write.
- Underrun, overrun, device-lost, invalid-format, unsupported-format, stale
  generation, and timeout results.
- Non-realtime control boundary versus realtime write boundary.

The boundary must not contain WASAPI or FFmpeg types.

### 5.2 Step 2: Deterministic fake clock and renderer

Implement:

- Manual clock advancement.
- Configurable render capacity.
- Deterministic consumption.
- Recorded lifecycle transitions.
- Bounded observation history.
- Explicit fault scheduling by write count, frame position, or generation.
- No sleeps as the primary synchronization mechanism in tests.

### 5.3 First mandatory fault set

Implement and test these before broadening the fake:

1. Stale-generation write.
2. Partial write.
3. Forced underrun.
4. Device lost during write.
5. Invalid or unsupported format.
6. Drain timeout.

### 5.4 Second fault set

After the first set is stable:

- Forced overrun.
- Delayed write.
- Write after stop.
- Write during seek or flush.
- Reset during write.
- Format change.
- Source-lost signal propagation.
- Slow-consumer pressure.
- Queue pressure.
- Command cancellation.
- Clock drift.

### 5.5 Exit criteria

- The fake implements the same platform-neutral boundary intended for WASAPI.
- All fault scenarios are deterministic and repeatable.
- Tests do not depend on wall-clock sleep timing.
- Generation mismatch naturally rejects stale data.
- Partial-write ownership is unambiguous.
- Drain and flush have explicit completion and timeout behavior.
- Failure observations are available without synchronous realtime logging.

---

## 6. Milestone 2: P0-F/P0-H First Real WASAPI Output

**Order:** Adapter shell, then real output immediately
**Purpose:** Prove that the architecture can drive an actual Windows audio
device before decode integration adds complexity.

### 6.1 WASAPI adapter shell

The platform adapter owns:

- COM apartment and object lifetime.
- Device enumeration and stable device identity.
- Shared-mode capability query.
- Exclusive-mode capability reservation, without requiring full support yet.
- Format-support mapping.
- Device invalidation mapping.
- Buffer alignment and device-period details.
- Device generation.
- Reopen policy surface.
- Platform error translation.

No COM pointer or Windows audio structure may cross into core contracts.

### 6.2 Minimum real output path

Use generated or embedded PCM, not FFmpeg, for the first real output proof:

```text
PCM generator
  -> preallocated audio blocks
  -> bounded SPSC render queue
  -> realtime render loop
  -> WASAPI shared-mode device
```

The first implementation should favor a single well-understood format such as
stereo float32 or stereo int16 at the device mix rate. Unsupported requests
must be reported explicitly.

### 6.3 Realtime requirements

- Event-driven WASAPI where supported by the selected path.
- No allocation in the steady-state render loop.
- No FFmpeg calls.
- No synchronous log formatting.
- No host callback from the render thread.
- Bounded queue and bounded observation record.
- Explicit silence/underrun behavior.
- Correct handling of partial device availability.

### 6.4 Required real-device tests

- Open and close the default device.
- Start, render audible PCM, stop, and reset.
- Repeat start/stop cycles.
- Observe underrun count.
- Reject an unsupported format clearly.
- Handle device invalidation without process crash.
- Run continuously for at least 30 minutes during early development.
- Extend to at least 60 minutes before P0-H closeout.

Hardware-dependent checks may be separated from deterministic CI tests, but
their result and environment must be recorded.

### 6.5 Exit criteria

- Audible generated PCM reaches a real device.
- The render path uses the same core boundary proven by the fake backend.
- Shared/exclusive status is visible and truthful.
- Device generation changes on reopen or invalidation.
- Device loss cannot leave dangling platform objects.
- Unsupported features never report success.

---

## 7. Milestone 3: P0-G/P0-I FFmpeg Local-File Loop

**Order:** Layered decode seam, then immediate end-to-end loop
**Purpose:** Decode a real local audio file and render it through the proven
output path without leaking library-specific types.

### 7.1 Decode layers

Keep these responsibilities separate:

- Byte source/provider.
- Container and demux session.
- Packet production.
- Codec decode session.
- Decoded audio block production.
- Media probe result.
- Seek capability.
- Gapless metadata extraction.
- Decode generation.
- Conversion request.

`AVFormatContext`, `AVCodecContext`, `AVPacket`, `AVFrame`, and FFmpeg error
codes remain inside the FFmpeg adapter.

### 7.2 First supported scope

- Local files only.
- One or a small documented set of common codecs and containers.
- Explicit source-open and decode failures.
- Explicit format negotiation result.
- Explicit conversion or rejection.
- Seek and flush may initially return a defined unsupported result.
- No network, NAS, WebDAV, cloud credentials, media library, cover art, or UI
  metadata.

### 7.3 End-to-end path

```text
local file source
  -> demux
  -> decode
  -> platform-neutral decoded blocks
  -> optional explicit conversion
  -> bounded queue
  -> WASAPI render
```

### 7.4 Required correctness

- Source and decode generations are carried with blocks.
- Old-generation blocks are rejected after source replacement.
- Format mismatch never truncates or reinterprets samples.
- Conversion reason is observable.
- EOS drains correctly.
- Source or decode failure cannot deadlock the render thread.
- Decoder starvation is distinguishable from device underrun.

### 7.5 Exit criteria

- A real local audio file plays to completion.
- FFmpeg and WASAPI types remain quarantined.
- Format negotiation is visible.
- Unsupported seek/flush states are explicit.
- EOS, source failure, decode failure, and device failure are distinguishable.
- At least one integer PCM and one floating-point or converted path is tested
  when supported by the chosen codec scope.

---

## 8. Milestone 4: P0-J Product State and Recovery

**Purpose:** Turn the minimum loop into a controllable playback core that
survives user races and device/source failure.

Implement in this order:

1. Session ownership and lifecycle.
2. Serialized command intake.
3. Pause and resume.
4. Seek, scoped flush, and clock reset.
5. Rapid seek coalescing or cancellation.
6. Track/source replacement.
7. EOS and drain completion.
8. Device-lost recovery and reopen.
9. Source/decode recovery classification.
10. Shutdown during active operations.

### 8.1 Required race scenarios

- Continuous seek.
- Rapid next-track commands.
- Pause during track switch.
- Seek during recovery.
- Close during drain.
- Shutdown during write.
- Source disconnect during seek.
- Device loss during flush.
- Old command arriving in a new session.
- Old audio block arriving after seek or source replacement.

### 8.2 State-machine rule

The state machine decides control behavior outside the realtime callback. The
render thread consumes already-decided commands or snapshots and does not run
complex transition policy.

### 8.3 Exit criteria

- All supported commands have ordering and cancellation semantics.
- Stale commands and blocks cannot mutate the new session.
- Pause position freezes according to contract.
- Seek resets decode, queue, render, and clock state by explicit scope.
- Device loss either recovers or terminates with a classified error.
- Close succeeds from failed and recovering states.
- Race tests pass deterministically under the fake backend.

---

## 9. Milestone 5: P0-K Commercial Stability

**Purpose:** Establish evidence that the core behaves reliably outside the
developer's primary machine.

### 9.1 Device matrix

At minimum:

- Integrated/onboard audio.
- USB DAC.
- Bluetooth.
- HDMI or DisplayPort audio.
- Default-device change.
- Device unplug/replug.
- Sleep and resume.

Record device identity, driver version where available, mode, format, result,
underruns, recovery, and known limitations.

### 9.2 Media corpus

Cover:

- WAV/PCM.
- FLAC.
- MP3 including VBR.
- AAC in a supported container.
- Large files.
- Truncated and corrupt files.
- Unusual sample rates.
- Mono, stereo, and multichannel where supported.
- Whole-file album/CUE inputs as explicit unsupported or reserved behavior
  until implemented.

Every committed or downloaded test asset must have license status and checksum.

### 9.3 Endurance and stress

- Continuous 1-hour development test.
- Continuous 10-hour release-candidate test.
- Repeated open/play/stop/close.
- Repeated seek and track replacement.
- Queue pressure and decoder starvation.
- Device change during playback.
- Memory and handle growth monitoring.

### 9.4 Exit criteria

- No unbounded memory or handle growth.
- No unrecovered deadlock.
- Failure rates and known device limitations are documented.
- Corrupt media produces a classified failure rather than process instability.
- Performance and underrun metrics have defined reporting units.

---

## 10. Milestone 6: P0-L Gapless and Timeline Accuracy

**Purpose:** Provide sample-accurate continuity rather than UI-level
approximation.

Required work:

- Decoder delay and encoder padding.
- Preroll and prefetch.
- Track segment and virtual-track boundaries.
- Cross-track timeline continuity.
- Whole-file album/CUE segmentation.
- Sample-domain rendered and audible position policy.
- EOS transition without inserted or dropped samples.

Exit criteria:

- Gapless fixtures have known expected sample boundaries.
- Consecutive compatible tracks do not add unintended silence.
- Timeline remains monotonic across a gapless transition.
- Position semantics identify decoded, queued, rendered, and estimated audible
  positions.
- Incompatible format transitions are explicit and tested.

---

## 11. Milestone 7: P0-M Exclusive and Bit-Perfect Truth

**Purpose:** Implement exclusive output and make bit-perfect a provable state.

Required proof inputs:

- Requested output mode.
- Actual WASAPI mode.
- Source, render, and device formats.
- Windows audio-engine participation.
- Resampler participation.
- Integer/float conversion.
- Channel remix.
- ReplayGain.
- Volume.
- DSP.
- Dither or other sample mutation.

Exit criteria:

- Exclusive mode is negotiated and handled independently of shared mode.
- Bit-perfect active requires all truth conditions to pass.
- Rejection reports a stable reason.
- Unknown conditions cannot be reported as active.
- Shared mode is described honestly.
- Hardware verification methodology is documented.

---

## 12. Milestone 8: P0-N Processing Boundary

**Purpose:** Add user-facing processing without compromising bypass truth.

Implement:

- Software volume.
- ReplayGain.
- Resampler boundary and quality policy.
- Channel remix policy.
- DSP chain ownership.
- Processing latency and tail handling.
- Dither policy where applicable.
- Strict bypass for bit-perfect operation.

Exit criteria:

- Every processor reports participation.
- Bypass does not mutate samples.
- Processing order is documented and tested.
- ReplayGain and software volume are disabled or rejected for bit-perfect.
- Resampling reason, source rate, target rate, and quality mode are visible.

---

## 13. Milestone 9: P0-O Host ABI

**Purpose:** Expose a stable product boundary only after core behavior is
proven.

Requirements:

- Versioned C ABI.
- Opaque handles.
- Size/version fields for extensible structures.
- Capability negotiation.
- Explicit ownership and callback-thread rules.
- No C++, STL, COM, Windows SDK, or FFmpeg types in ABI.
- Host-serialized commands.
- Sanitized diagnostics.
- Source resolver and credential handling remain host responsibilities.

Exit criteria:

- ABI compatibility tests.
- Invalid handle and shutdown tests.
- Host cannot directly manipulate render/decode queues.
- Callback threading and lifetime are documented.
- Tauri integration is an adapter, not core ownership.

---

## 14. Milestone 10: P0-P Release Hardening

Required release gates:

- Signed binaries and installer.
- Reproducible build record.
- FFmpeg build configuration and provenance.
- Third-party notices and SBOM.
- Crash diagnostics with privacy redaction.
- Symbol handling.
- Version and compatibility policy.
- Upgrade and rollback behavior.
- Release device/media/endurance matrix.
- Commercial claim review.

No release is approved only because playback works on the development machine.

---

## 15. Deferred Research

### P0-Q Hi-Res, DSD, and DoP

Research only until PCM, stability, and truth reporting are mature. Contract
design must not make future support impossible, but speculative implementation
must not delay the primary PCM path.

### P0-R mpv/libmpv

Reference and benchmark first. Any compatibility mode must be independently
isolated, labeled, observed, licensed, and excluded from the primary core's
bit-perfect and gapless claims.

---

## 16. Quality Gates by Maturity

### Before P0-D/P0-E

- Clean MSVC build.
- Contract and capability tests.
- Public-header standalone and composition checks.
- Active repository gates.

### Before P0-H

- Deterministic fake failure matrix.
- Queue and generation tests.
- No-platform-leak checks.
- Realtime boundary review.

### Before P0-I

- Real WASAPI generated-PCM proof.
- Device invalidation behavior.
- Render-loop allocation and blocking audit.

### Before P0-K

- End-to-end local-file playback.
- State/race/recovery tests.
- Basic performance metrics.

### Before P0-P

- Static analysis.
- Sanitizer-supported builds where toolchain support permits.
- Fuzzing for parsers and adapter boundaries.
- Device, media, endurance, and recovery matrices.
- Dependency, license, provenance, privacy, and signing gates.

---

## 17. Metrics That Must Become Visible

The implementation should expose bounded, unit-defined observations for:

- Frames decoded, queued, submitted, and estimated consumed.
- Queue fill and low-water mark.
- Underrun and overrun counts.
- Device and render period.
- Decode and source latency outside realtime.
- Device, stream, source, seek, and flush generations.
- Clock drift.
- Format renegotiation count.
- Device reopen count.
- Recovery attempts and outcomes.
- State transition count.

Metrics are diagnostic evidence, not permission to add synchronous realtime
logging.

---

## 18. Change Control

This roadmap may be amended when implementation evidence reveals a better
sequence. An amendment must include:

1. The proposed sequence change.
2. The implementation evidence requiring it.
3. Affected contracts, tests, phases, and release claims.
4. Migration or compatibility impact.
5. Owner approval.
6. Updated roadmap version and effective date.

The following are not sufficient reasons to change the sequence:

- A desire to create more directories.
- A preference for a new abstraction without a consumer.
- A formatting cleanup.
- A demo shortcut that bypasses the shared boundary.
- A platform or library type that is convenient to expose through core.

---

## 19. Immediate Work Queue

Execute these items in order:

1. Lock the P0-N processing family and strict-bypass ownership.
2. Implement software volume and ReplayGain without hidden activation.
3. Implement explicit resampling and channel-remix boundaries.
4. Implement processor participation, latency, tail, and dither truth.
5. Prove byte-preserving bypass and bit-perfect incompatibility.
6. Implement the versioned P0-O host ABI.
7. Complete P0-P release, dependency, privacy, and commercial hardening.

---

## 20. Final Execution Sequence

```text
P0-C closeout hardening
  -> minimal render boundary
  -> deterministic fake backend and fault injection
  -> WASAPI adapter shell
  -> generated PCM real output
  -> layered FFmpeg seam
  -> local-file decode-to-output loop
  -> state, seek, race, and recovery hardening
  -> commercial stability matrix
  -> sample-accurate gapless
  -> exclusive mode and bit-perfect truth
  -> ReplayGain, volume, resampler, and DSP
  -> versioned host ABI and Tauri adapter
  -> commercial release hardening
  -> deferred Hi-Res and compatibility research
```

This sequence is the default path for future implementation work.
