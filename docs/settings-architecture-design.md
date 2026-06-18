# Kivo Music — Settings Architecture (Definitive Design)

**Status:** Design / awaiting product-owner decisions (§8). Supersedes the current flat Settings page.
**Date:** 2026-06-16
**Scope:** the full Settings surface + its persistence/registry spine + the extensibility seam for all
named future features (AI, 网盘/cloud, 资料库/library, 语言/i18n, account/sync, audio/DSP, playback, privacy,
advanced).
**Grounding:** verified against the live codebase — every path, pattern, and bug below is confirmed, not assumed.
**Methodology:** produced via 构思 → 规划设计 (multi-perspective research of best-in-class players → three
independent IA proposals → synthesis). This document is the synthesis. Implementation does NOT begin until the
§8 product decisions are made (Phases 0–1 can begin immediately; they are decision-independent).

---

## 0. What the code actually looks like today (verified)

| Fact | Evidence |
|---|---|
| Settings is one flat `Flickable + Column`, ad-hoc sections | `KivoMusicQt/ui/qml/pages/settings/SettingsPage.qml` |
| Hardcoded hexes leak the design system | `#e5e5ea` / `#f0f0f2` (borders/hover), `#ffe5e5` / `#d70015` (remove) in `SettingsPage.qml` |
| Strings are an untranslatable EN/中文 mix, zero `qsTr()` | `"添加来源"`, `"扫描"`, `"已添加"` vs `"Dark Mode"`, `"Audio Output Device"` |
| The "Output Device: Default" row is a dead `Text` label | `SettingsPage.qml` — no model, no action |
| **Theme is NOT a singleton** — a plain `QObject` instantiated per file in **47 files** | `ui/qml/tokens/Theme.qml` is `QObject{}`; `Theme { id: theme }` everywhere → `theme.toggle()` flips one local copy ⇒ **dark mode is globally broken** |
| Routing is a string-keyed Loader; privacy/eula/about are **top-level `currentPage` peers** | `ui/qml/windows/HomeWindow.qml` `getPageComponent()` switch |
| First-run EULA is a **Dialog**, NOT route-coupled (Q6 resolved) | `App.qml` `eulaDialog` opened by `checkFirstRun()`; only the Settings *link* uses `currentPage==="eula"` |
| Canonical persistence pattern | `native/sources/roots/user_source_root_store.{hpp,cpp}` — `load()/save()` over `QSettings` (`HKCU\Software\Kivo\Kivo Music`) |
| Canonical controller pattern | `native/sources/roots/source_manager_controller.hpp` — `Q_PROPERTY` view + `Q_INVOKABLE` mutators + `*Changed` signal |
| Side-effect wiring pattern | `app/main.cpp` — `setContextProperty(...)` ×6; `sourcesChanged → libraryScanService.start(...)` |
| **AudioCore C ABI has NO device enumeration (Q5 resolved)** | `kivo_audio_api.h` exports only create/destroy/get_capabilities/get_snapshot/get_truth/open_source/pump/submit_command |
| Homeless-but-built capabilities needing a settings home | `audio_core_bridge/eq/`, `snapshot/truth_snapshot`, `queue/{play_queue,playback_mode}`, `lyrics/`, `services/library_index/`, `data/database/` |

---

## 1. Decisions (the synthesis)

1. **IA: audiophile-/media-hub-first, with friendly framing.** Kivo's identity is a bit-perfect engine + media
   hub, not a streaming client. Foreground Sources / Library / Audio Engine / DSP / Playback; demote
   Account / AI / Cloud to a clearly-labelled secondary tier — but **every page must read friendly**
   (plain-language labels, "Show advanced" tiering, gate-don't-hide).
2. **Shell mounts via the existing `currentPage` route.** Keep `HomeWindow`'s `currentPage === "settings"` branch;
   it loads a new **`SettingsShell.qml`** (old `SettingsPage.qml` is deleted). The shell owns its *own* internal
   navigation (`currentCategoryId` + optional `entityRoute`); it does **not** add new top-level route peers.
3. **Privacy/EULA/About stop being top-level route peers** → entity sub-routes under **About & Legal**, hosting
   the existing `PrivacyPolicy.qml` / `EulaPage.qml` / `AboutPage.qml` files **unchanged** via an internal
   StackView. Safe because first-run EULA is a Dialog (Q6).
4. **One typed `SettingsStore` (generalizing `UserSourceRootStore`) + a `SettingsCatalog` registry + thin
   per-domain controllers.** Not a god-object: the store is pure persistence; each domain gets its own small
   controller mirroring `SourceManagerController` exactly.
5. **Theme-as-singleton is Phase 0 and blocks everything.** 47 files move from `Theme { id: theme }` to the
   imported singleton. This single refactor *is* the dark-mode fix.
6. **Search + deep-link + reset-to-default are first-class from day one**, all derived from the one catalog so
   they never drift.
7. **Secrets never touch QSettings.** A Windows `CredentialStore` (wincred/DPAPI) holds tokens/keys; the store
   persists only a non-secret handle/flag. Built when Cloud/Account/AI first need it; the seam is reserved now.

---

## 2. Top-level category set — NOW vs LATER

Four rail super-sections; each category = one `SettingsCatalog` descriptor + one self-contained page.

### 媒体中心 / Media Hub (primary)
| Category (id) | NOW (migrate/wire existing) | LATER |
|---|---|---|
| **Sources** `sources` | `sourceManager.roots` + `discovery` LAN scan, as a first-class entity list (local/NAS/SMB) + per-source detail | Cloud source *types* (WebDAV, S3, OneDrive/GDrive, Navidrome/Jellyfin/Subsonic) as entity rows |
| **Library** `library` | Scan/watch-folder policy, cover-art, SQLite DB location/rebuild | Tag editing, dedup, organize/rename, import rules, smart-playlist defaults, per-source overrides |
| **Audio Engine** `audio` | Replace dead "Output Device" label; WASAPI exclusive/bit-perfect; ReplayGain/pre-amp; embedded **live `truth_snapshot`**; explicit **Apply** | Real device list (**blocked on ABI, Q5**), sample-rate/bit-depth strategy, DSD, per-device profiles |
| **DSP & EQ** `dsp` | Existing EQ as one processor in an ordered chain | Resampler, dither, convolution, crossfeed (drop-in chain processors) |
| **Playback** `playback` | Repeat/shuffle, queue, lyrics, mini-player defaults, volume | Gapless/crossfade, autoplay/auto-mix |

### 系统 / System (primary)
| Category | NOW | LATER |
|---|---|---|
| **Appearance** `appearance` | Dark Mode (now global), accent/density/startup/tray | System-follow theme, font scaling, reduce-motion |
| **Language & Region** `language` | Begin `qsTr()` + `QTranslator`; locale picker; RTL-ready layout | Live switch, lyrics-language, region/number/date formats |
| **Network** `network` | Discovery toggle, proxy, timeouts | Bandwidth/metered limits, per-protocol tuning |
| **Storage & Cache** `storage` | Cache size/location, clear (confirm) | Offline/pinned-download location, eviction policy |
| **Shortcuts** `shortcuts` | Read-only map (from `KeyboardShortcuts.qml`) | Remappable hotkeys, media keys, per-binding reset |

### 账户与服务 / Account & Services (secondary)
| Category | NOW | LATER |
|---|---|---|
| **Account & Sync** `account` | Present-but-empty anchor (signed-out) | OAuth sign-in (token in CredentialStore), settings sync, scrobbling, backup/restore |
| **Cloud** `cloud` | Reserved; may fold under Sources initially (§8.2) | Remote backends as connect cards + per-backend sync/cache |
| **AI** `ai` | Master switch OFF / opt-in; provider stub; consent line | Recommendations, auto-tag, AI lyrics, AI DJ, semantic search |
| **Privacy** `privacy` | Telemetry/consent (default off), manage stored credentials | Crash reporting, data-retention/clear-history |

### 诊断 / Diagnostics (footer)
| Category | NOW | LATER |
|---|---|---|
| **Advanced & Diagnostics** `advanced` | Raw `truth_snapshot` dump, logs/level, reset-all, engine/ABI version | Experimental flags, searchable expert-knob tree |
| **About & Legal** `about` | Hosts existing About/EULA/Privacy as sub-routes | OSS attributions, update channel/check |

---

## 3. The Natural Family Tree (concrete paths)

### 3.1 QML — shell + reusable vocabulary
```
ui/qml/tokens/
  Theme.qml                         # PROMOTE to `pragma Singleton`
  qmldir                            # NEW: `singleton Theme 1.0 Theme.qml`

ui/qml/pages/settings/
  SettingsShell.qml                 # NEW root: Row(rail + detail host); owns currentCategoryId
                                    #   + entityRoute; openCategory(id, entityId). REPLACES old page.
  shell/
    SettingsCategoryRail.qml        # left rail: search field + grouped category ListView (catalog-bound)
    SettingsSearchResults.qml       # flat filtered leaf results that replace the rail while searching
    SettingsDetailHost.qml          # right pane: async Loader keyed off currentCategoryId + StackView
                                    #   drill-in + optional Apply/Revert bar
  categories/
    sources/SourcesPage.qml         # migrates sourceManager.roots + discovery (entity list)
    sources/SourceDetailPage.qml
    library/LibraryPage.qml
    audio/AudioEnginePage.qml       # device picker (gated until ABI) + exclusive/bit-perfect + live truth + ReplayGain
    dsp/DspChainPage.qml
    dsp/EqProcessorPage.qml         # wraps existing EqPanel/EqSlider
    playback/PlaybackPage.qml
    appearance/AppearancePage.qml   # drives Theme singleton; fixes dark mode
    language/LanguagePage.qml
    network/NetworkPage.qml
    storage/StoragePage.qml
    shortcuts/ShortcutsPage.qml
    account/AccountPage.qml         # placeholder anchor
    cloud/CloudPage.qml             # reserved (§8.2)
    ai/AiPage.qml                   # master toggle + provider stub
    privacy/PrivacyPage.qml
    advanced/AdvancedPage.qml
    about/AboutLegalPage.qml        # hosts existing AboutPage/EulaPage/PrivacyPolicy
  PrivacyPolicy.qml / EulaPage.qml / AboutPage.qml   # UNCHANGED, reused
  # DELETE SettingsPage.qml after migration verified

ui/qml/components/settings/         # reusable row vocabulary — Theme tokens ONLY (kills hardcoded hexes)
  SettingsSection.qml SettingsToggleRow.qml SettingsChoiceRow.qml SettingsSliderRow.qml
  SettingsActionRow.qml SettingsNavRow.qml SettingsEntityListRow.qml SettingsAuthCard.qml
  SettingsOrderedProviderList.qml SettingsResetButton.qml
```
`SettingsEntityListRow` generalizes today's `sourceManager` delegate and is reused for sources, output devices,
cloud backends, and AI providers — the single most important reusable idea (Roon two-tier entity-list pattern).

### 3.2 C++ — store + registry + per-domain controllers
```
native/services/settings/
  settings_store.{hpp,cpp}          # generalizes UserSourceRootStore: typed get/set over QSettings;
                                    #   per-key defaults; scope tag; reset(key)/resetAll(). Pure persistence.
  settings_scope.hpp                # enum Machine | Portable | PerEntity
  settings_catalog.{hpp,cpp}        # registry QObject; Q_PROPERTY QVariantList categories
                                    #   {id,title,group,tier,icon,qmlSource,keywords,weight}. context "settingsCatalog".
  settings_controller.{hpp,cpp}     # facade "settingsController": Q_INVOKABLE get/set/reset
  controllers/
    appearance_settings_controller.{hpp,cpp}   # theme mode/accent/density
    audio_settings_controller.{hpp,cpp}        # output/exclusive/bit-perfect/ReplayGain; *Changed → reconfig
    playback_settings_controller.{hpp,cpp}     # repeat/shuffle/queue/lyrics defaults
    # LATER (additive): library_/cloud_/ai_/account_ controllers
  credentials/
    credential_store.{hpp,cpp}      # Windows Credential Manager (wincred/DPAPI); built when first needed

native/audio_core_bridge/output/
  output_device_enumerator.{hpp,cpp}   # BLOCKED on ABI (Q5) — ships when the C ABI adds enumeration
```
All new controllers register as context properties in `app/main.cpp` beside the existing six; audio reconfig
wires through the verified `*Changed → reconfigure` pattern.

---

## 4. The uniform extensibility seam

A new feature ships its settings with **three additive moves and ZERO shell edits**:
1. **Drop one page** under its own subfolder (e.g. `categories/ai/AiPage.qml`) composed only from the shared
   `Settings*` row vocabulary; add its line to `CMakeLists.txt` `QML_FILES`.
2. **Register one descriptor** by appending to `settings_catalog.cpp`
   (`{ id:"ai", title:qsTr("AI"), group:"services", tier:"secondary", qmlSource:"categories/ai/AiPage.qml",
   keywords:[...], weight:80 }`). Rail, grouping, **and** search index rebuild automatically.
3. **Optionally add one controller** under `controllers/` following `SourceManagerController` exactly, registered
   in `main.cpp`. Persist via `SettingsStore`; secrets via `CredentialStore`.

Within a category, growth is data-driven: cloud backends, DSP processors, AI providers, and watch-folders are all
lists of entities rendered by `SettingsEntityListRow` / `SettingsOrderedProviderList` + a per-entity detail page.
This is the VS Code `contributes.configuration` / foobar2000-plugin model expressed in Qt: the shell is a registry
host; features self-register.

---

## 5. Cross-cutting mechanisms

- **Search-in-settings:** non-empty rail search swaps the category list for `SettingsSearchResults` (built from
  catalog `keywords` + each page's declared leaf labels); click → `openCategory(id, anchorKey)` + highlight.
  Index rebuilds from the active locale's strings.
- **Deep-linking:** `SettingsShell.openCategory(categoryId, entityId)`; the truth panel's "configure output" and
  an empty-library "Add a source" jump straight in.
- **Persistence scope (Machine | Portable | PerEntity):** every key tagged at write. Output device / cache path /
  DB location / window geometry = Machine (never synced); theme / EQ presets / shortcuts / language = Portable.
  Structural cure for the global-state bug class; makes future sync/backup non-breaking.
- **Reset-to-default:** every key carries a default; `SettingsResetButton` shows a modified indicator + reset at
  control/page/app scope. Destructive ops confirm.
- **Apply semantics:** live-apply by default; heavyweight audio reconfig uses an explicit **Apply/Revert** bar.
- **Gate, don't hide:** prereq-gated controls render disabled with an inline reason; placeholder categories ship
  present-but-disabled with "coming soon / sign in to enable".

---

## 6. Phased roadmap

- **Phase 0 — Foundations (BLOCKER, no UI yet).** Promote `Theme.qml` to `pragma Singleton` (+ `tokens/qmldir`
  + CMake flag); replace all **47** `Theme { id: theme }` with the singleton import (*this fixes global dark
  mode*). Stand up `settings_store` + `settings_catalog` + `settings_controller`; register context properties.
  Green build, no behavior change beyond dark mode working.
- **Phase 1 — Shell + migrate the 4 existing sections (feature parity, scalable).** Build `SettingsShell` + rail
  + reusable `Settings*` rows; point `HomeWindow`'s `"settings"` branch at it. Migrate with no behavior loss:
  Appearance → `AppearancePage`; dead Output row → `AudioEnginePage`; Music Sources + discovery → `SourcesPage`;
  Legal → `AboutLegalPage` sub-routes (demote privacy/eula/about route peers; update the Settings link). Delete
  old `SettingsPage.qml`. Ship search-in-settings.
- **Phase 2 — Foreground the audiophile core.** Flesh out `AudioEnginePage` (exclusive/bit-perfect + embedded live
  truth + ReplayGain; device list gated on ABI), `DspChainPage`/`EqProcessorPage`, `PlaybackPage`, `LibraryPage`.
  Wire `audio`/`playback` controllers.
- **Phase 3 — System + secondary tiers as registered placeholders.** Language & Region (begin i18n), Network,
  Storage & Cache, Shortcuts, Account anchor, AI stub, Privacy, Advanced & Diagnostics. Add `credential_store`,
  reset-to-default, backup/restore.
- **Phase 4 — Incremental feature drop-in.** Cloud backends + richer AI land per §4 — each is one catalog
  descriptor + one page (+ optional controller). Shell, rail, and search never change.

---

## 7. Trade-offs accepted

Phase 0 is a wide, must-be-correct 47-file refactor before any visible settings benefit; ~25 new QML + ~10 C++
files (justified by the roadmap); `output_device_enumerator` is **blocked on an ABI addition (Q5)** — Audio page
keeps a gated "System Default" until then; placeholder pages risk a "coming soon" feel if not gated cleanly;
`qsTr()` rollout is a cross-cutting retrofit; `credential_store` is Windows-only.

---

## 8. Decisions required from the product owner

**Resolved by code inspection:**
- **Q5 — output-device enumeration:** the AudioCore C ABI does **not** expose it. Real device selection is a
  **backend (KivoAudioCoreCpp) task**; frontend ships a gated "System Default" placeholder. Not a shell blocker.
- **Q6 — first-run EULA:** a Dialog, not route-coupled. Demoting eula/privacy/about to sub-routes is safe;
  handled in Phase 1.

**Resolved by product owner (2026-06-16):**
1. **Account/sync:** NOT now. Account is a disabled placeholder anchor; no cross-device sync infra built.
2. **Cloud:** folds in as new source *types* under **Sources** (WebDAV, Navidrome/Jellyfin, …); no standalone
   Cloud category activation yet.
3. **AI:** **bring-your-own-key** — user supplies a provider API key stored via the OS credential manager
   (DPAPI), never QSettings. AI category is a Phase 3+ placeholder until built.
4. **Languages:** ship **English + 简体中文** — full `qsTr()` rollout + `QTranslator` + locale switcher + zh_CN
   translations. RTL: layout stays RTL-ready but no RTL locale ships now.

**Build approved:** Phase 0 + Phase 1 — proceed.
