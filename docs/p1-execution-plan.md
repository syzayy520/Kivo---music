# P1 Execution Plan (adversarially vetted)

**Source:** `kivo-p1-design` workflow (13 agents; realtime / ABI-contract /
boundary-truth lenses). **ABI:** one coordinated **1.0.0 → 1.1.0** MINOR bump
(additive only; MAJOR/PATCH/STRUCT_VERSION_1 unchanged). Execute in order;
build+verify after each cluster (P1-1 → P1-3/4 → P1-2).

## Verified facts that shape the work
- `successful_drains` increments at **exactly one** site —
  `playback_runtime_drain.cpp:54` in `complete_end_of_stream`, reached only from
  `render_step` on natural EOS. STOP/SEEK/FLUSH/recovery never touch it. It is a
  cumulative engine-lifetime counter. `eos_drain_tests.cpp:44` already asserts it
  fires exactly once per natural completion. → edge-trigger on strict increment.
- `endOfStream` is a **same-thread DirectConnection** (lifecycle.cpp:20) →
  `onEndOfStream → openFile` runs synchronously re-entrant inside the poll.
  **Must set baseline BEFORE emit.** Bounded to one nesting (QTimer non-reentrant).
- ABI snapshot has **no duration/rate**; volume command **does not exist**.
- `default_open_configuration.cpp:30-31` ALREADY sets volume enabled, gain=0.7
  (the original "fixed unity" premise was wrong).
- **NO per-field atomics** for gain state: chain `plan_`/`snapshot_` are plain
  members; `process()` (writer-adjacent reader), `set_volume`, and `snapshot()`
  all take the coordinator `Impl::mutex_`; the WASAPI worker never calls the
  chain. The single mutex makes the tuple tear-free; atomics would *add* a race.

## Steps

### P1-1 — EOS auto-advance (bridge only, NO ABI change)
1. **STEP 1** `snapshot/playback_snapshot.{hpp,cpp}`: add `uint64_t successfulDrains`
   to `PlaybackSnapshot`; in reader set it from `abiSnapshot.successful_drains`
   (field already populated; reader already requests v1+sizeof).
2. **STEP 2** `controller/audio_playback_controller_impl.hpp` + `_snapshot.cpp` +
   `_open.cpp`: add `uint64_t lastSuccessfulDrains_`; promote `static int
   detailPollCounter` → member `detailPollCounter_`. In `pollSnapshot` after
   reading snapshot: `if (drains > lastSuccessfulDrains_) { lastSuccessfulDrains_
   = drains; emit owner_->endOfStream(); }` (baseline BEFORE emit). In `openFile`
   before `startSnapshotPolling()`: seed `lastSuccessfulDrains_` from a fresh read
   and set `detailPollCounter_ = kDetailPollEvery` (kills ~450ms stale-badge).
3. **STEP 3** `_queue.cpp`: on terminal `queueFinished` set `bitPerfectStatus_` to
   idle/CoreReady + reset `detailPollCounter_` (truth quiesce). Document the
   ">1 drain between polls collapses to one advance" bounded contract + test.

### P1-3/4 — real duration + sample rate (ABI tail extension)
4. **STEP 4** core `playback_runtime_snapshot.hpp` + `coordinator_state.hpp` +
   `playback_runtime_open.cpp` + `playback_runtime_snapshot.cpp`: add
   `total_frames`, `total_frames_known`, `source_sample_rate`, `resample_active`;
   capture from the validated probe under the existing `scoped_lock` on open;
   reset on rollback/close; copy in `snapshot()` under `mutex_`.
5. **STEP 5** ABI: append to TAIL of `kivo_audio_snapshot_v1` (after
   `source_replacements`, exact order, two uint32 before uint64):
   `render_sample_rate, total_frames_known, total_frames, source_sample_rate,
   resample_active`. struct_version stays 1 (additive). Add capability
   `KIVO_AUDIO_CAPABILITY_PLAYBACK_TIMEBASE_SNAPSHOT = 1ull<<9` and OR into mask;
   bump `ABI_VERSION_MINOR 0→1`. Map fields in `host_audio_engine_snapshot.cpp`
   before the memcpy. Frozen prefix (BASE_SIZE) untouched.
6. **STEP 6** bridge `playback_snapshot.{hpp,cpp}` + `_commands.cpp`: add
   `renderSampleRate/totalFrames/hasDuration`; **gate on the capability bit**
   before trusting the tail. Replace `/48000` and `duration=ms(0)`; progress
   denominator decodedFrames→totalFrames. `seekTo`/`skipBy` use
   totalFrames/renderSampleRate; no command when unknown; clamp to totalFrames-1.

### P1-2 — real runtime volume (ABI command extension)
7. **STEP 7** new `include/kivo/core/processing/gain/unity_tolerance.hpp`
   (genealogy: beside gain/): `constexpr double kUnityTolerance=1e-12;
   constexpr bool differs_from_unity(double)`. Update `audio_processing_plan.cpp`
   to include it + delete the local copy (single source of truth).
8. **STEP 8** core chain/producer/coordinator `set_volume` (plain members under
   `mutex_`, NO atomics). `AudioProcessingChain::set_volume(double) noexcept`:
   clamp finite [0,1]; recompute `effective_gain, volume_active, strict_bypass,
   AND dither_active` together into BOTH `plan_` and `snapshot_`. Producer →
   Impl → chain. Coordinator::set_volume mirrors seek; dispatch under `mutex_`,
   reject if closed/inactive/no producer, clamp before persisting
   `producer_configuration_.processing.volume`, then `producer_->set_volume`.
   Do NOT add SetVolume to `execute()` switch. Entire chain noexcept.
9. **STEP 9** ABI `kivo_audio_command.h` + `host_audio_engine_command.cpp`:
   append `KIVO_AUDIO_COMMAND_SET_VOLUME=8`; doc `KIVO_AUDIO_VOLUME_GAIN_FIXED_ONE
   =(1u<<24)` — `requested_frame` carries Q40.24 gain ONLY for SET_VOLUME. Decode
   to double in `submit()` (overload invisible to core), `INVALID_ARGUMENT` if
   !finite/out-of-range, route to `runtime_.set_volume` NOT `execute()`. Keep
   non-throwing (the C entry has no try/catch).
10. **STEP 10** bridge `command/volume_command.{hpp,cpp}` + `_commands.cpp` +
    `state/default_open_configuration.cpp`: `toFixedGain(double) noexcept` (clamp
    + Q24 via llround); `setVolume` submits `SET_VOLUME` when engine open + value
    changed (still emits volumeChanged; no-engine path still updates). Open-time
    gain = controller's CURRENT `volume_` (not hardcoded 0.7), keep enabled=1.
11. **STEP 11** `docs/architecture/host_abi_integration_contract.md`: ABID
    1.0.0→1.1.0; document SET_VOLUME requested_frame Q40.24 reuse, the snapshot
    tail (0==unknown), and the new capability bit. Truth scope: `render_sample_rate`
    is the processed/negotiated rate (not bit-perfect); `source_sample_rate +
    resample_active` expose the resample fact per audio_truth_policy 6.5.

## Governance / risks to honor
- Q24 near-unity rounding can exceed 1.0 → trips `SoftwareVolumeConfiguration::
  is_valid()` (>1.0 rejected) → silent producer drop on next seek/recovery.
  Defensive clamp to [0,1] at encode, ABI boundary, runtime persist, and chain.
- Header/DLL skew: capability bit gates the tail (old DLL → "--:--").
- One new file only (unity_tolerance.hpp) — genealogy clean, no flat dump. Every
  new field wired + tested (no-empty-forest). Core never gains Qt/Windows.h/avcodec
  (decode-to-double in submit keeps `kivo_audio_*` out of core).
- Tests: extend `tests/{host_abi,playback_runtime,playback_pipeline,
  audio_processing}` + Qt bridge tests (see workflow result for the full list).
