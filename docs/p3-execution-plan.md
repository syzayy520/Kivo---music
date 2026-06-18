# P3 Execution Plan — Threading + Data Architecture (adversarially vetted)

**Source:** `kivo-p3-design` workflow (13 agents; thread-safety / boundary /
feasibility lenses). **No AudioCore/ABI change** (verified unnecessary).

## Decisions
- **SQLite: WIRE (minimal).** The `library` table + `PlaylistDao` exist but are
  never written and `migrate()` is a no-op. Add a per-thread `LibraryRepository`
  (upsert/read the existing **v1** table) + an honest `migrate()` (reads
  MAX(version); v1==latest is a tested no-op). DEFER the full normalized
  tracks/albums/artists schema + a multi-step migration runner (gold-plating).
- **Threading: TWO new threads, not the doc's five.** (1) a **library worker**
  QThread (scan + cover-glob + SQLite writes on its own `kivo_library` connection;
  produces a value-typed `QList<MusicFileRecord>` handed to the UI thread via one
  queued signal, where models update on the UI thread). (2) `QtConcurrent::run`
  for open-time cover discovery (session-generation guarded). **The ABI pump +
  snapshot poll STAY on the UI-thread QTimer** — verified correct: `snapshot()`/
  `truth()` block on `command_mutex_` while `pump()`/`submit()` try-lock→BUSY, so a
  worker reading snapshots would *contend* with UI-thread command submission. (The
  doc's "audio control worker" lane is a higher-risk Phase B, deferred.)

## Hard thread rules honored
QAbstractListModel mutations + QML-bound `*Changed()` emits run ONLY on the UI
thread. Workers produce value types and deliver via `Qt::QueuedConnection` /
`QMetaObject::invokeMethod`. SQLite connections are per-thread (by name). Workers
are `moveToThread`'d QObjects with no parent; threads `quit()`+`wait()` in the
owner's dtor BEFORE dependent state is destroyed.

## Steps

### STEP 1 — Move the startup scan off the UI thread *(biggest win, ship first)*
- `music_file_record.h`: `#include <QMetaType>` + `Q_DECLARE_METATYPE(MusicFileRecord)`.
- NEW `native/services/library_index/library_scan_worker.{hpp,cpp}`: a `QObject`
  (no parent) with slot `run(roots, maxFiles)` → `MusicFileScanner::scan` →
  `emit scanFinished(QList<MusicFileRecord>)`. No GUI/model types.
- NEW `native/services/library_index/library_scan_service.{hpp,cpp}`: UI-affinity
  `QObject` owning a `QThread` + `moveToThread`'d worker; `start()` posts the run
  via `invokeMethod(...Qt::QueuedConnection)`; re-emits `resultsReady` on the UI
  thread; dtor `quit()+wait()`.
- `library_model_bundle.{h,cpp}` + `search_model_bundle.{h,cpp}`: ctor seeds demo
  unconditionally; add `applyMusicRecords(const QList<MusicFileRecord>&)` (UI-thread
  begin/endResetModel) for real data.
- `app/main.cpp`: `qRegisterMetaType<MusicFileRecord>()` + `<QList<MusicFileRecord>>()`;
  remove the synchronous scan; construct empty bundles (self-seed demo); declare a
  stack-local `LibraryScanService`; connect `resultsReady` → `applyMusicRecords`;
  `service.start(defaults(), 768)`.
- `CMakeLists.txt`: add the 4 new files. Verify: window paints instantly; library
  lands shortly after.

### STEP 2 — Open-time cover I/O off the UI thread
- `audio_playback_controller_impl_open.cpp`: remove the inline `MusicCoverFinder::
  findFor` block; dispatch via `QtConcurrent::run` capturing path + sessionGeneration;
  deliver to a new UI-thread slot `applyResolvedCover(gen, url)` (writes
  `coverArtPath_` + emits only if `gen == current sessionGeneration` — drops stale).
  Keep the open/openSource/play sequence inline (sub-ms). `extractMetadata` stays
  (pure string parse, no I/O). Add `Qt6::Concurrent` to CMake.

### STEP 3 — SQLite persistence (WIRE, minimal)
- NEW `native/services/library_index/library_repository.{hpp,cpp}`: ctor takes a
  connection NAME; `upsertTracks` (INSERT ON CONFLICT(file_path)); `allTracks()`.
  Worker opens `kivo_library` on its thread, upserts, closes in `QThread::finished`.
- `database_manager.{hpp,cpp}`: honest `migrate()` (MAX(version); `kLatestSchemaVersion=1`).
- `main.cpp`: `migrate()` synchronously before service.start; seed bundles from a
  UI-thread `LibraryRepository("kivo_main").allTracks()` (instant); empty → demo.

### STEP 4 — Artwork cache hardening (isolated; NO scheme flip)
- `cover_image_provider.cpp`: decode OUTSIDE the lock (reserve key under lock →
  release → `QImageReader::read()` → re-acquire to insert); fix evict off-by-one
  (`>=` → `>`). Provider registration + QML `image://artwork` flip is DEFERRED
  (Artwork-lane follow-up; doc "UI never reads artwork" stays UNMET until then).

### STEP 5 — Test infra + tests
- Root `CMakeLists.txt` has no `add_subdirectory(tests)`/`enable_testing()`, and
  `tests/CMakeLists.txt`'s `add_kivo_test` links only `Qt6::Core`. Wire the subtree
  (link Sql/Gui/Concurrent, AUTOMOC) before adding: library-scan service test,
  DB-migration idempotence test, artwork-cache bounds test.

## Governance
New `native/services/library_index/` subfamily (doc-blessed `services/` tree). Do
NOT create `services/audio_control/` (no audio-control worker in P3) nor an empty
`image_provider/artwork/` tree. AudioCore stays untouched (no forbidden-token risk
there). Pre-existing drift to flag (not deepen): scanner lives under
`native/sources/music/` (absent from the doc tree) — TODO relocate under services/.
