# Windows Backend Completion Program

**Document ID:** KIVO-WINDOWS-BACKEND-COMPLETION-001
**Status:** ACTIVE EXECUTION BASELINE
**Last Updated:** 2026-06-13

## Purpose

Define the remaining engineering and external evidence needed for a truthful
Windows commercial backend closeout.

## Current Phase

P0-C through P0-R repository work is committed. Post-R observability,
installer handoff, device evidence, endurance evidence and commercial
regression workstreams are active.

## Objective

Close the Windows C++ backend as a truthful commercial system, not merely a
passing library build. Work repeats this cycle:

```text
design -> natural family tree -> implementation -> static structure audit
       -> accumulated local validation -> evidence update -> next admitted item
```

No phase is complete because a document says so. Completion requires the named
code, tests, artifacts and external evidence.

## Current Truth

- P0-C through P0-P repository-controlled engineering work is implemented.
- PR #1 contains the committed P0-C through P0-P engineering baseline.
- P0-Q has Hi-Res PCM contracts and explicit native DSD/DoP rejection, but no
  DSD-to-PCM, DoP or native DSD playback.
- P0-R has only an external executable probe. No pinned mpv binary, libmpv
  integration or compatibility mode exists.
- The host ABI has an additive extended diagnostic snapshot while retaining
  the original v1 prefix size for older hosts.
- AudioCore has full-payload clean/upgrade/rollback verification, but no
  product installer or clean-VM transaction evidence.
- The device probe emits UTF-8 JSON and creates the required physical test
  rows, but all unexecuted rows remain `not_run`.
- Endurance tooling samples process resources and runtime failure counters,
  but no ten-hour physical release run has been completed.
- Commercial regression intake and closure records are machine validated, but
  no real user-feedback campaign can be manufactured in this repository.
- Commercial release remains blocked by certificate, source/legal custody,
  product installer, physical hardware, endurance and user-feedback evidence.

## Completion Workstreams

### S0 - PR #1 Engineering Closeout

Repository controlled:

- keep PR title and description aligned with actual P0-C through P0-P scope
- review the 34-commit accumulated patch for critical regressions
- require clean build, full CTest, nine repository gates and diff hygiene
- record that GitHub CI and independent review are absent until configured
- keep the PR Draft until an explicit merge decision

Exit: no stale docs-only/build-blocked claim and no unresolved P0 defect.

### S1 - P0-Q/P0-R Truth Repair

Repository controlled:

- DSD codec ids map to explicit DSD identity
- native DSD rejects before entering the PCM decoder/converter path
- DoP contract distinguishes 24 valid bits from 24/32-bit transport container
- DoP cadence and cross-channel marker synchronization are explicit
- mpv probe uses null-output untimed mode and records discovered version
- benchmark approval requires pinned binary hash, media hash, options, elapsed
  time, exit classification and provenance

Exit: research tools cannot be mistaken for playback support or reproducible
benchmark proof.

### S2 - Production Observability

Repository controlled:

```text
include/kivo/core/observability/
  category/
  snapshot/
src/core/playback/runtime/
  recovery/
  resource/
  snapshot/
src/host/abi/
  diagnostic/
  engine/
tests/observability/
  scenario/category/
  scenario/snapshot/
```

Required monotonic counters:

- render underruns and protocol overruns
- queue full/oversized rejections and producer backpressure
- device invalidations and reopen attempts/successes/failures
- format negotiation and renegotiation attempts/outcomes
- decode failures by stable category
- recovery attempts/successes/failures/fallback stops
- seeks, drains, stale generations and source replacements

Rules:

- realtime paths increment preallocated atomic or owner-thread counters only
- no logging, allocation, formatting or callbacks on realtime paths
- exported snapshots are numeric, bounded, versioned and privacy-safe
- product telemetry upload, consent, retention and sampling remain host owned
- ABI evolution is additive; existing v1 structure sizes remain accepted

Exit: counters are observable through a versioned host ABI snapshot and tested
under fault injection without exposing paths or media identity.

### S3 - FFmpeg Corresponding Source and Legal Evidence

Repository plus external custody:

```text
tools/release/compliance/
  source/
    acquisition/
      acquire_ffmpeg_source.ps1
    archival/
      archive_ffmpeg_corresponding_source.ps1
    foundation/
      source_bundle_foundation.ps1
    verification/
      verify_ffmpeg_source_bundle.ps1
    testing/
      test_ffmpeg_source_bundle.ps1
docs/dependencies/
  ffmpeg/
    binary_provenance.md
    source_custody_record.md
    legal_review_record.md
```

Repository automation must download or accept the exact source revision,
verify revision/configuration/hash, include build scripts and notices, and
produce a deterministic source archive manifest. A supplied verified archive
is copied beside the RC and bound into its build manifest and release index.
It must never fabricate legal
approval. Legal name, reviewer, scope, date and approved markets are external
signed records.

Exit: binary and corresponding source are traceably paired; legal approval is
stored outside secrets and linked from the release evidence.

### S4 - Authenticode

Repository controlled tooling, externally supplied certificate:

- SHA-256 Authenticode for DLL and installer
- RFC 3161 timestamp
- certificate subject/thumbprint/expiry/EKU capture
- post-sign verification and tamper-negative test
- no PFX, password, private key or signing token in repository/logs
- CI signing identity separated from developer engineering builds
- independent trusted-chain, SHA-256, timestamp and expected-thumbprint
  verification for final artifacts
- tamper-negative Authenticode verifier self-test with an ephemeral,
  non-exportable test certificate

Exit: valid signatures verified on final bytes. Until certificate access
exists, status remains `BLOCKED: CERTIFICATE`.

### S5 - Product Installer Transactions

Product-repository owned, AudioCore provides payload contract:

- signed clean install on supported Windows baselines
- upgrade from prior known-good release
- failed-upgrade rollback
- full-set atomicity for Kivo DLL and FFmpeg DLLs
- all ABI handles closed before replacement
- post-install hash/signature/startup smoke verification
- uninstall and user-data preservation policy

AudioCore verifier:

```text
tools/release/installer/
  foundation/
  verification/
  testing/
```

Exit: clean install, upgrade and rollback each have machine-readable manifests
and logs from clean virtual machines. AudioCore alone cannot mark this passed.

### S6 - Physical Device Matrix

External hardware lab with repository probe tooling:

- built-in endpoint
- at least two USB DAC classes and driver variants
- Bluetooth A2DP reconnect and profile changes
- active HDMI/display audio
- default-device switch during play/pause/seek/drain
- unplug/replug, disable/enable and driver restart
- sleep/resume and display power transitions
- shared/exclusive, supported/unsupported rates and channel layouts

Each row records OS build, device/driver identity, requested/accepted format,
mode, duration, underruns, invalidations, reopen result and evidence hash.
Absent hardware is `NOT PRESENT`, never `PASS`.

Repository tooling:

```text
tests/hardware/device_matrix/
  fixture/
  platform/windows/
  serialization/
  scenario/
tools/validation/device_matrix/
```

### S7 - Ten-Hour Endurance

Repository tooling plus physical machine:

- pinned release candidate and media corpus
- continuous 10-hour playback
- periodic seek, pause/resume and track transition schedule
- memory/handle/thread/GDI growth sampling
- underrun, backpressure, invalidation and recovery snapshots
- final artifact hashes and machine/device identity

Exit: no unbounded resource growth, crash, deadlock, audible-stall-class event
or unexplained counter increase. A shorter run is development evidence only.

The v2 evidence records private bytes, working set, handles, threads, GDI/USER
objects, CPU time, underruns, overruns, device invalidations and worker wait
timeouts. Ten hours without a matching device evidence hash remains review
incomplete.

### S8 - Commercial Feedback Regression

Product and support owned:

```text
issue -> privacy-safe reproduction -> stable failure classification
      -> deterministic test or hardware row -> fix -> regression release
      -> evidence link -> support closure
```

Required fields: affected release, OS/device class, sanitized diagnostic
snapshot, reproducibility, severity, owner, test id, fix commit and released
version. User media and personal paths are not attached by default.

Repository tooling:

```text
tools/validation/regression/
  foundation/
  new_commercial_regression_record.ps1
  validate_commercial_regression_record.ps1
```

Exit: every confirmed commercial defect has a durable regression proof or a
documented reason why only physical-lab evidence is possible.

### S9 - Performance and Format Expansion

Only after S2 observability:

- startup/open/seek latency distributions
- decode/render CPU and allocation profiles
- queue occupancy and jitter evidence
- FLAC/ALAC/AAC/MP3/Opus/Vorbis/WAV matrix expansion
- multichannel and uncommon sample layout policy
- DSD-to-PCM as a visibly converted path
- DoP/native DSD only after endpoint and processing-bypass proof

Optimization without before/after measurements is forbidden.

### S10 - Mobile

Mobile is a separate platform program, not a Windows WASAPI subfolder:

- shared platform-neutral contracts may be reused
- Android output belongs under an Android adapter project/family
- Apple output belongs under an Apple adapter project/family
- platform build, lifecycle, interruption, route and store compliance are
  independent gates

Windows commercial closeout must not be delayed or falsely completed by mobile
planning.

### S11 - LyricsCore

Lyrics follows `docs/architecture/lyrics_core_boundary.md` and lives outside
AudioCore. Product orchestration joins lyric timeline selection to versioned
AudioCore position snapshots. No parser/provider/cache code enters realtime
audio families.

## Execution Order

1. Keep S0/S1 closed under accumulated regression validation.
2. Complete S2 full-build and ABI validation.
3. Finish repository portions of S3/S4 and preserve blocked truth.
4. Hand the implemented S5 verifier to the product installer repository.
5. Run external certificate, legal, VM, hardware and endurance evidence.
6. Use the implemented S8 record gate during commercial rollout.
7. Optimize and expand formats only from measured evidence.
8. Start mobile and LyricsCore as separate projects with their own gates.

## Final Definition

`WINDOWS_BACKEND_ENGINEERING_COMPLETE` requires all repository-controlled
items and tests.

`WINDOWS_BACKEND_COMMERCIAL_RELEASE_APPROVED` additionally requires valid
signatures, source/legal records, installer transaction proof, physical device
matrix, ten-hour endurance and claim review.

`PERFECT` is not a release classification. The durable target is a backend
whose behavior, limitations, failures and evidence are complete enough that no
claim depends on trust or hidden assumptions.

## Allowed

- Narrow workstream implementation in the ownership trees defined above.
- Explicit blocked classifications for unavailable external evidence.
- Additive ABI and release-tool evolution backed by tests.

## Gate

Every admitted slice requires design ownership, genealogy pass, targeted tests,
full accumulated validation and updated evidence before the next slice closes.

## Failure

The program fails if an external requirement is fabricated, realtime rules are
weakened, ABI compatibility is broken, or a capability claim exceeds evidence.

## Future Phase

After Windows commercial approval, mobile adapters, additional device classes,
format expansion and LyricsCore proceed as independently gated programs.

## Deferred

Certificate procurement, legal opinion, installer implementation, physical
hardware availability, store submission and real user feedback cannot be
completed solely inside this repository.
