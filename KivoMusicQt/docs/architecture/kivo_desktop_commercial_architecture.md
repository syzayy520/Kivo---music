# Kivo Desktop Commercial Architecture

**Document ID:** KIVO-DESKTOP-COMMERCIAL-ARCHITECTURE-001
**Status:** ACTIVE PLANNING BASELINE
**Effective Date:** 2026-06-13

## Verdict On Proposed Stack

The proposed stack is directionally correct:

```text
Qt 6 + Qt Quick/QML
C++ ViewModel / QAbstractListModel
SQLite
C++ cache + QML ImageProvider
C++ lyrics parser + QML visual renderer
CMake + Ninja + MSVC
```

It is not yet complete enough for a top commercial desktop music product. The
stronger version below adds ownership boundaries, realtime safety, evidence
truth, diagnostics, packaging, test gates, and future extension lanes.

## Final Stack

```text
Kivo Desktop
|-- App shell: Qt 6 + Qt Quick/QML
|-- UI logic: C++ QObject ViewModels + QAbstractListModel families
|-- Audio boundary: Product C++ AudioCoreBridge over Kivo Host ABI
|-- Audio core: KivoAudioCoreCpp DLL, no Qt/QML dependency
|-- Playback output: AudioCore-owned WASAPI/renderer families
|-- Media library: SQLite + product indexing service
|-- Metadata: C++ tag scanner service, isolated from UI thread
|-- Artwork: C++ artwork cache + QQuickImageProvider
|-- Lyrics: separate LyricsCore parser/timeline + QML visual renderer
|-- Devices: product device model over AudioCore snapshots/evidence
|-- Diagnostics: product health model + engineering evidence panel
|-- Settings: product settings service, schema-versioned
|-- Packaging: Qt deployment + signed installer + crash/privacy policy
`-- Build: CMake + Ninja + MSVC
```

## Non-Negotiable Boundaries

- QML never calls AudioCore, FFmpeg, WASAPI, SQLite, or file scanners directly.
- UI thread never pumps AudioCore, scans files, reads artwork, parses lyrics,
  or touches blocking I/O.
- AudioCore never imports Qt, QML, SQLite, product UI state, artwork, or lyrics
  rendering.
- Playback truth comes from AudioCore evidence and product bridge snapshots,
  not from UI state.
- LyricsCore stays separate. The product joins lyric timeline data to AudioCore
  playback position above native realtime boundaries.
- Media library state is product state. It is not an AudioCore responsibility.

## Natural Family Tree

```text
apps/desktop-qt/
  app/
    main.cpp
    application_environment.cpp
  native/
    audio_core_bridge/
      loader/
      handle/
      command/
      snapshot/
      diagnostic/
    models/
      playback/
      library/
      queue/
      devices/
      diagnostics/
      lyrics/
    services/
      library_index/
      metadata_scan/
      artwork_cache/
      settings_store/
      task_scheduler/
    image_provider/
      artwork/
  ui/
    qml/
      App.qml
      windows/
      pages/
        home/
        library/
      features/
        playback/
        library/
        queue/
        devices/
        diagnostics/
        settings/
        lyrics/
      components/
        navigation/
        transport/
        truth/
        artwork/
        library/
        table/
        empty_state/
      tokens/
      effects/
  tests/
    cpp/
    qml/
    integration/
    screenshots/
packages/
  design/
    tokens/
    references/
    review_notes/
  audio_contract/
    commands/
    snapshots/
    diagnostics/
  packaging/
    installer/
    signing/
    notices/
```

## Thread Model

```text
Qt UI thread
  QML rendering, input, lightweight ViewModel property changes only

Audio control worker
  Host ABI open/command/pump/snapshot sequencing

Library worker
  SQLite writes, scanning, metadata parsing, index updates

Artwork worker
  image decode, resize, cache fill, QQuickImageProvider backing store

Lyrics worker
  parse, alignment, translation preparation, timeline enrichment
```

No worker owns another worker's state. Cross-thread updates are message-based
and bounded.

## Database Plan

Use SQLite for product media-library state:

```text
tracks
albums
artists
album_artists
track_artists
playlists
playlist_items
sources
scan_roots
artwork_blobs
lyrics_entries
play_history
settings
schema_migrations
```

Rules:

- UI reads through models, not raw SQL.
- Migrations are explicit and versioned.
- Large artwork is cached by content hash or stable media identity.
- Paths and credentials stay out of AudioCore diagnostics.

## Artwork Plan

- C++ cache owns file decode, embedded cover extraction, resizing, and memory
  bounds.
- QML uses provider URLs such as `image://artwork/<id>/<size>`.
- Missing artwork uses designed placeholders, not broken images.
- Image loading is asynchronous and never blocks transport controls.

## Lyrics Plan

- Parser and provider logic live outside AudioCore.
- QML owns visual rendering only: current line, next line, translation,
  karaoke progress, and empty/error states.
- The product maps lyrics to AudioCore position snapshots after playback state
  is copied out of realtime/native code.

## Audio Truth Surface

The frontend must have explicit UI slots for:

```text
active device
requested output mode
actual output mode
sample rate
bit depth
container bits
exclusive/shared
engine in path
conversion active
processing active
bit-perfect verdict
evidence incomplete
```

No badge may claim Hi-Res, DSD, DoP, bit-perfect, or gapless unless the
corresponding evidence is present.

## Commercial Gates

```text
cmake configure/build
Qt C++ unit tests
QML tests
model/view contract tests
SQLite migration tests
artwork cache bounds tests
lyrics parser tests
screenshot regression tests
keyboard navigation tests
accessibility/contrast audit
installer/signing validation
crash/privacy redaction validation
device matrix smoke
endurance run
```

## First Real Implementation Order

1. Qt shell and design token system.
2. Static home screen with component families.
3. C++ mock playback/library/device models.
4. QML views consume C++ models only.
5. SQLite schema and read-only library browser.
6. Artwork image provider and cache.
7. AudioCoreBridge mock, then real Host ABI loader.
8. Device and truth panels.
9. LyricsCore boundary and visual renderer.
10. Packaging, signing, diagnostics, and endurance gates.

## Readiness Verdict

```text
KIVO-DESKTOP-QT6-COMMERCIAL-ARCHITECTURE-LOCKED-001
QT6_QML_CPP_SQLITE_IMAGEPROVIDER_LYRICSCORE_AUDIOCORE_HOST_ABI_BOUNDARIES_APPROVED
```
