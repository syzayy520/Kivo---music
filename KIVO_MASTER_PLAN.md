# Kivo Music — Master Plan (Living Document)

**Status:** ACTIVE · **Owner:** engineering · **Last updated:** 2026-06-16
**Supersedes:** `audit_report*.md`, `Kivo_Music_Code_Audit_Report.md`, `issue_list_and_fix_plan.md`, `r.md`, `audit_part1.md`, `store_readiness_gap_analysis.md` (all stale — archive/delete).

This is the single source of truth for getting Kivo Music to a complete,
store-ready commercial product. It is maintained continuously.

---

## 0. Operating Method (every task follows this loop)

```
构思 → 规划设计 → 自然家族树 → 实现 → 单文件单职责
     → 不能平铺文件（能子文件夹的必须子文件夹）→ 检查并修复 → 循环
```

- Design before code. Place files by **genealogy** (natural family tree), never
  bucket/flatten. One file = one responsibility. Use subfolders whenever a
  family emerges. Build + verify each change before looping.
- Governance gates stay green: genealogy gate, no-empty-forest, forbidden-token
  (core layer never imports Windows.h/avcodec.h/mpv), layer dependency direction.

---

## 1. Audit Verdict (2026-06-16)

| Layer | Rating | Summary |
|---|---|---|
| `KivoAudioCoreCpp` (C++ audio core) | ★★★★★ | Genuinely top-tier: enforced boundaries, RT-safe lock-free render, evidence-gated bit-perfect, real tests, no stubs. |
| `KivoMusicQt/native` (bridge/data) | ★★ | Single-threaded prototype in a "multi-threaded commercial" shell; several headline features wired but **dead**. |
| `KivoMusicQt/ui` (QML) | ★★ | Apple-Music-grade facade, but the **audio-truth surface** (the product's whole reason to exist) is hardcoded fiction and never shown. |

**Core problem:** the backend computes the truth honestly; the frontend never
displays it, and the bridge between them is broken & full of dead wiring.

---

## 2. Roadmap (phases)

### 阶段 0 — 止血 / Stabilize  *(DONE — build green 2026-06-16)*
- [x] P0-1 Fix broken build: `audio_playback_controller.hpp` had `#include`
      **inside** the namespace. Fix v1 (forward-decl) was wrong — Qt 6.5 moc
      static_asserts that `Q_PROPERTY(PlayQueue*/PlaybackMode*)` pointee types
      are fully defined. Final fix: move the includes **above** the namespace
      (full definitions, never `#include` inside a namespace).
- [x] P0-V Qt build verified green (0 errors, exe linked; Qt 6.5.3/msvc2019,
      VS2022 generator, `out/local-build`).
- [x] P0-2 Cleaned stale root artifacts → `docs/archive/superseded-2026-06-16/`;
      deleted transient `*_build.txt`/`*.log`; fixed `run.ps1` to use
      `KIVO_QT_ROOT`/Qt 6.5.3 + auto-locate exe; added log patterns to .gitignore.

> **Toolchain note:** project was authored for **Qt 6.8.3/msvc2022** (used
> `QuickEffectsPrivate`); only **Qt 6.5.3/msvc2019** is installed, so the build
> was migrated down (QuickEffectsPrivate removed, QTP0004 gated to ≥6.8). Decide
> later whether to install 6.8.3 for full visual-effects fidelity (owner decision).

### 阶段 0.5 — Runtime smoke  *(DONE)*
- [x] P0-T Upgraded to **Qt 6.8.3 LTS** (win64_msvc2022_64) in `.qt/`; CMakeLists
      restored `QuickEffectsPrivate` (UI uses `MultiEffect` pervasively); clean
      configure + full build green (exe 1.86 MB, MSVC 19.44/VS2022).
- [x] P0-S `windeployqt` + AudioCore/FFmpeg DLLs deployed next to exe; app
      **launches and runs without crashing** (alive >7s). Toolchain validated
      end-to-end (builds + runs).

> **Runtime QML findings (observed in P0-S stderr; fix during P1/P4):**
> - Binding loop on `audioController` in `HomeWindow.qml:44` (FileDropArea) and
>   `App.qml:35` (KeyboardShortcuts) — fold the fix into P1/P2 audioController wiring.
> - `Unable to assign [undefined] to QColor` at `SidebarBrand.qml:15` — broken
>   color binding (likely missing Theme token); fix in P4 token enforcement.

### 阶段 1 — 播放端到端可用 / Playback works end-to-end  *(DONE — 2026-06-16)*
Vetted by the `kivo-p1-design` workflow; executed per `docs/p1-execution-plan.md`.
ABI **1.0.0 → 1.1.0** (additive). 17/17 core tests pass (incl. 2 new chain
volume tests); both projects build green; app runs; governance gates green.
- [x] P1-1 EOS auto-advance: bridge edge-triggers on the ABI `successful_drains`
      counter (natural completion only) → emits `endOfStream` → queue advances.
      Baseline seeded in `openFile`, set before emit (re-entrant DirectConnection).
- [x] P1-2 Real volume: new **`KIVO_AUDIO_COMMAND_SET_VOLUME`** (gain as Q40.24 in
      `requested_frame`) wired ABI → coordinator `set_volume` (new `runtime/volume/`
      family) → session (state-neutral `SetVolume` case) → producer → chain
      `set_volume` (re-runs `build_audio_processing_plan`; recomputes gain/
      participation/bypass/dither as a tuple under the coordinator mutex — NO
      per-field atomics). Bridge `setVolume` submits it; `openFile` opens at the
      live volume. Refuses non-unity under bit-perfect.
- [x] P1-3/4 Real duration + sample rate: core captures probe duration/source-rate;
      ABI snapshot grew a timebase tail (`render_sample_rate`, `total_frames[_known]`,
      `source_sample_rate`, `resample_active`) gated by a new capability bit; bridge
      consumes real rate/duration, unifies `seekTo`/`skipBy` on the render-rate basis.

> **Deviations from the vetted plan (justified):** (1) STEP 7 (extract a shared
> `unity_tolerance.hpp`) was SKIPPED — `chain::set_volume` delegates to
> `build_audio_processing_plan`, which already owns the tolerance, so the logic is
> shared by construction (no new file, stronger single-source-of-truth). (2) Env
> fix: the AudioCore's FFmpeg root pointed at a dead `Administrator` profile;
> reinstalled FFmpeg 7.1 LGPL for the current user + repointed `KIVO_FFMPEG_ROOT`.
> (3) Pre-existing latent C4127 in `compatibility_tests.cpp` (constant-condition
> assert) surfaced on recompile → converted to `static_assert`. (4) Registered the
> new `runtime/volume/` subfamily in `check_genealogy_gate.ps1` (genealogy-correct).

### 阶段 2 — 点亮产品灵魂：音频真相面板 / Audio-truth surface  *(DONE — 2026-06-16)*
**Verified:** AudioCore + Qt build green; 17/17 core tests + 2 new host_abi truth
tests pass; 3 governance gates green; app runs clean (no new QML errors). The
product now displays the truth the engine proves — and "Unknown" when it can't.
Vetted by the `kivo-p2-design` workflow; executed per `docs/p2-execution-plan.md`.
**Crux finding:** the bit-perfect evidence/verdict pipeline was DEAD CODE (tests
only). ABI **1.1.0 → 1.2.0** (additive).
- [x] **Step 1** Core: wired `build_bit_perfect_evidence` + `evaluate_bit_perfect_truth`
      into the runtime snapshot (inputs captured at open under `mutex_`, reset on
      release; built off the realtime thread). Linked `kivo_output_truth` into the
      runtime lib.
- [x] **Step 2** ABI: new dedicated flat `kivo_audio_truth_v1` struct (presence
      `known_mask`/`bool_mask` with pinned bit indices, verdict + frozen
      rejection-category enums, 3 flat format descriptors), version bump,
      `KIVO_AUDIO_CAPABILITY_BIT_PERFECT_TRUTH = 1<<10`.
- [x] **Step 3** Host: `kivo_audio_get_truth` getter + adapter flattener
      (`host_audio_engine_truth.cpp`, `static_assert`-pinned enum contract),
      exported in the `.def`, capability advertised. **17/17 core tests green.**
- [x] **Step 4** Bridge: optional `getTruth` resolution; structured verdict-gated
      `TruthSnapshot` + honesty helpers (`isBitPerfectConfirmed()`); 11 truth
      Q_PROPERTYs on the controller; poll/store/emit (outside the lock).
- [x] **Step 5** QML: `TruthBadge` (verdict chip, gold only for exact
      "Bit-Perfect"), new `TruthRow`, rewritten `SignalPathPanel` bound to real
      props (collapses to "Unknown — evidence incomplete"), `SignalPathPopover`
      reachable from the transport status area, `PlayingPulse` replacing the RNG
      fake spectrum, removed the unevidenced "bit-perfect" copy in App.qml.
      New host_abi tests assert idle ⇒ evidence-incomplete (never bit-perfect).

> Honesty law enforced end-to-end: "Bit-Perfect" shown only when
> `evidenceComplete && verdict==BitPerfect && exclusive && !engine && !conversion
> && !processing`; Unknown when evidence incomplete or the capability is absent.

### 阶段 3 — 兑现架构承诺 / Honor the documented architecture  *(DONE — 2026-06-16)*
Vetted by `kivo-p3-design` (thread-safety/boundary/feasibility lenses); see
`docs/p3-execution-plan.md`. **No AudioCore/ABI change.** Two new threads (not the
doc's five); pump deliberately stays on the UI timer (moving it would contend on
`command_mutex_`). Build green; app runs through the SQLite path with no crash/SQL
errors; the runtime QML warning my change exposed was fixed.
- [x] **P3-1 (STEP 1)** Startup scan moved off the UI thread — new
      `native/services/library_index/` (`LibraryScanWorker` + `LibraryScanService`,
      moveToThread, queued `resultsReady`); bundles seed demo in the ctor and swap
      real data via `applyMusicRecords()` on the UI thread → window paints instantly.
- [x] **P3-1 (STEP 2)** Open-time cover lookup off the UI thread via
      `QtConcurrent` + `applyResolvedCover` (sessionGeneration stale-drop + QPointer).
- [x] **P3-2 (STEP 3)** SQLite **wired** (minimal): per-thread `LibraryRepository`
      (upsert/read the existing v1 table), worker persists on its own connection,
      honest `migrate()` (MAX(version); v1==latest no-op); cold launch seeds the real
      library from the DB instantly, then the async rescan refreshes it.
- [x] **P3-3 (STEP 4)** `CoverImageProvider` hardened (decode outside the lock;
      evict off-by-one). NOTE: the `image://artwork/<id>` provider *registration* +
      QML scheme flip is intentionally **deferred** (Artwork-lane producer follow-up);
      covers still load via `file://` paths today.
- [ ] **STEP 5 (deferred → P4)** Wire the orphaned `tests/` subtree (no
      `add_subdirectory(tests)`; `add_kivo_test` links only `Qt6::Core`) and add
      library-scan / DB-migration / artwork-cache tests. Build-infra, not the
      threading/data feature — moved to commercial hardening.

### 阶段 4 — 商业打磨 + 发布门 / Commercial polish & release gates
- [ ] P4-1 Accessibility pass (Accessible.* + tooltips + focus rings).
- [ ] P4-2 Enforce token system (kill 89 hardcoded colors), fix dark mode,
      extract App.qml dialogs, replace string-compiled page transitions w/ StackView.
- [ ] P4-3 Backend: freeze normalization churn; add hostile-media fuzz + endurance;
      make MMCSS realtime path the shipping path (or prove pull-pump under load).
- [ ] P4-4 External gates: Authenticode signing, legal/source custody, installer
      (upgrade/rollback), device matrix, endurance soak.
- [ ] P4-5 (optional/competitive) Hi-Res/DSD playability (P0-Q).

---

## 3. Task Ledger

Tracked live via the session task list; mirrored here at phase close. Detailed
per-task books for the audio core remain under
`KivoAudioCoreCpp/docs/tasks/`.
