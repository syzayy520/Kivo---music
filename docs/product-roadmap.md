# Kivo Music — ROI-Ranked Roadmap to a Store-Ready Commercial Player

**Date:** 2026-06-16
**Method:** grounded multi-agent assessment of the *real* product state (6 dimensions, code-read,
real-vs-demo) → ROI ranking → top-item design. Payoff = (visible impact × reach) ÷ effort, penalizing
blocked work, rewarding credibility repair + unlock value.

**Recent updates folded in (already done this session):**
- ✅ Playback **stutter** root-caused (UI-thread producer pump) and **fixed** (dedicated `AudioPumpWorker`
  thread); engine confirmed clean (tone-smoke: 0 underruns). See [stutter task].
- ✅ Settings architecture Phase 0+1 (Theme singleton/global dark mode + navigable shell + 4 categories).
- ✅ NAS/SMB sources (manual + auto-discovery); self-contained deployment.
- ◻️ 10-hour WASAPI endurance soak: tooling verified working (a 15s cycle reported 0 underruns/overruns/
  timeouts/invalidations); full physical run still open.

---

## Verdict

**A ★★★★★ audio engine wearing a ★★ coat.** Almost nothing on the critical path is *missing* — decode,
WASAPI exclusive, bit-perfect truth, queue/mode logic, the SQLite playlist DAO, the settings spine, and
scanning are all **real and verified**. What's broken is the last 1–5% of wiring per feature, plus a cluster
of "looks like a demo" credibility holes that are fatal for a *paid* product. The highest-payoff work now is
**closing the gap between real backends and dead frontends** — each fix is S-effort and removes a visible lie.

## The single highest-payoff item right now

**Wire the shuffle/repeat buttons in `TransportBar.qml` + surface the active mode.** The glyphs render but
have no `onClicked`; the backend (`PlaybackMode` 4-state + `togglePlaybackMode()`, already exercised from the
queue panel) is fully real. ~5 lines of QML, zero ABI dependency, primary surface, every session. A dead
button on the always-visible transport bar is the fastest way a paid app feels fake.

> Design note (from the top-item design): the bar shows two buttons (shuffle, repeat) but the backend is one
> 4-state enum. Map two-button intent onto it via intent-named slots `toggleShuffle()` / `cycleRepeatMode()`.
> **Latent bug to fold in:** `onEndOfStream` auto-advances via `playQueue_->next()` directly and never calls
> `playbackMode_->nextIndex()` — so **Shuffle is ignored on auto-advance**. Wiring the button makes this
> user-reachable; fix it in the same change.

---

## ROI ranking

### Tier 0 — "Stop looking fake" (this week; all S, all unblocked, all wire-real-backends)
1. Wire shuffle/repeat + show active mode (TransportBar) — *primary surface; kills fake-button perception*
2. Show sample rate + bit depth in the now-playing card (bind `truthSampleRate`/`truthBitDepth`) — *the audiophile money shot, currently buried behind a panel*
3. Add a mini-player toggle button (`miniPlayer.show()`) — *MiniPlayer is fully built but unlaunchable*
4. Hide / "Coming Soon"-gate the EQ panel — *EQ is UI theater not wired to DSP; a fake EQ in a paid audiophile app is a refund/review risk*
5. Wire `SearchFilterModel` → real filtering (Songs/Albums/Artists) — *filter chips currently no-op*
6. Library sort toggle (Recently Added / Artist / Album / Title) — *shown as selectable; isn't*

### Tier 1 — Core product completeness
7. Queue persistence (QSettings save/restore) — S
8. Real playback history → Continue Listening (persist last-played/position) — S–M *(currently demo-seeded)*
9. **Playlist UI + `PlaylistController`** (Create/Rename/Delete/Manage) — M *(DAO is 100% real; zero UI/invokables)*
10. Favorites system (mark/unmark; reuse #9 infra) — M *(Favorites/Focus/Made-for-You are demo = all songs)*
11. Error-on-missing-file → auto-skip to next — S
12. ReplayGain enable/disable toggle in Settings → Audio — S

### Tier 2 — Polish ("works" → "commercial-grade")
13. Empty + loading states across all pages — M *(nothing renders for empty/scanning today)*
14. Migrate ~82 remaining hardcoded colors → Theme tokens — S–M
15. Lyrics synced to timeline (local `.lrc` only; online fetch is a blocked stub) — M
16. Accessibility foundation (`Accessible.role/name` + focus ring; currently zero) — M (do the S-slice first)
17. Settings input validation feedback (NAS path red/green/disabled) — S
18. Audio quality badges (lossless vs lossy; stored-metadata slice) — M

### Tier 3 — Store gates & release engineering
19. Version single-source-of-truth (CMake → manifest + QML) — S (do early)
20. Store assets (icons/tiles/screenshots; empty `store/Assets/` hard-fails packaging) — M
21. **i18n pipeline + wrap strings** (qsTr coverage ~2.3% today; lupdate/lrelease; QTranslator; zh_CN) — XL — *start wrapping NOW in parallel*
22. Code signing (Authenticode cert + signtool) — M — **BLOCKED: cert purchase**
23. FFmpeg LGPL source archival + custody record — M — **BLOCKED: legal**
24. Standalone installer (NSIS/WiX) if shipping outside MS Store — L — decision-dependent
25. Crash reporting, auto-update, SBOM, export-compliance, device-matrix — M–L — several externally blocked

### Blocked / descope (don't burn time now)
- **EQ → backend DSP** (XL, BLOCKED): no ABI command for band updates / no peaking filter. Hide it (Tier-0 #4); revisit via an ABI-extension project.
- **Device enumeration / exclusive-mode selection UI** (BLOCKED): ABI has no device enumeration; exclusive is negotiated per-open only.
- Online lyrics, curation ML, smart playlists, metadata editing — none on the critical path; defer.

---

## Recommended sequence (next 6 milestones)
1. **M1 — "Stop looking fake" (1 wk):** Tier-0 #1–6 + #19. Biggest credibility jump per hour.
2. **M2 — "Acts like a real player" (1–2 wk):** #7, #9, #10, #11, #8. **De-demo gate:** replace hardcoded pins/favorites/continue-listening.
3. **M3 — "Audiophile proof on screen" (3–5 d):** #12, #18, finish truth surfacing. *(= Settings Phase 2: advanced-audio panel.)*
4. **M4 — "Feels polished" (1–2 wk):** #13, #14, #15, #17, #16 (S-slice).
5. **M5 — "Localized":** #21 i18n — wrapping starts during M1–M4; pipeline + zh_CN lands here. *(= Settings Phase 3: language picker.)*
6. **M6 — "Shippable" (gated by externals):** #20, #22, #23, store automation. *(= Settings Phase 4: telemetry-consent/data-reset.)* **Start cert + legal procurement at M1** so they're not the long pole.

Settings Phase 2/3/4 are **not** a separate track — fold each panel into the milestone whose feature it exposes.

---

## Top credibility risks for a paid release (explicit flags)
1. **Fake EQ** shown as functional — hide before any paid build (Tier-0 #4).
2. **Demo data masquerading as real:** Home pins (`DemoLibrarySeed::pins()`), Favorites/Focus (= all songs), Continue Listening (2 seeded albums). De-demo is a **release gate** (M2).
3. **Dead primary-UI buttons:** shuffle/repeat — make the main bar honest first (Tier-0 #1).
4. **Mixed EN/中文 strings** while advertising bilingual support (M5).
5. **Empty `store/Assets/`** — hard packaging blocker (M6).
