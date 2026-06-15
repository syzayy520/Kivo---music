# P2 Model Wiring Gate

**Document ID:** KIVO-QT-P2-MODEL-WIRING-GATE-001
**Status:** ACTIVE IMPLEMENTATION CONTROL
**Effective Date:** 2026-06-14

## Commercial Intent

P2 turns Kivo Desktop from a static visual prototype into a product shell with a
real model boundary. The first slice must not attempt SQLite, NAS, WebDAV, cloud
drive, or AI. It must first prove that QML pages can consume stable C++ model
roles without owning demo arrays, business state, or future storage details.

## Design Verdict

Use C++ `QAbstractListModel` families as the product boundary for QML:

```text
QML page
  reads role names and counts only

C++ model
  owns list shape, roles, reset semantics, and future update contract

Seed/source layer
  owns temporary demo content now, then SQLite/source services later
```

This avoids three future rewrites:

- replacing QML arrays with models later,
- renaming role fields after UI hard-codes array positions,
- coupling NAS/WebDAV/cloud/AI source state directly to pages.

## Natural Family Tree

```text
native/models/library/
  album_list_model.h/.cpp
    one album-like grid model: albums, pins, recent album groups
  artist_list_model.h/.cpp
    one artist grid model
  track_list_model.h/.cpp
    one track-like row model: songs, videos, playlist rows
  library_model_bundle.h/.cpp
    one QML-facing composition root for albums, pins, recent, artists, tracks
  demo_library_seed.h/.cpp
    temporary seed data only, removable when SQLite is ready

ui/qml/pages/library/
  LibraryPage.qml
    layout and mode selection only
```

## QML Rules

- QML consumes `model.roleName`, not `modelData[index]`.
- QML may read `model.count` for summaries.
- QML may choose which model to display based on page mode.
- QML may not own library data arrays after this slice.
- QML may not call SQLite, scanners, network sources, or AudioCore.

## C++ Rules

- Each model class owns one role family.
- Model bundle composes models only; it does not scan files or query SQL.
- Temporary seed data is isolated in `demo_library_seed`.
- QObject member models are owned by C++ member lifetime, not parent deletion.
- Role names are stable because future SQLite/source adapters must feed them.

## Future Replacement Path

```text
demo_library_seed
  -> local SQLite read model
  -> source-index service
  -> NAS/WebDAV/cloud source adapters
  -> AI enrichment review models
```

The QML page should not change when the source of model rows changes.

## Acceptance Evidence

```text
family gate pass
no LibraryDemoData QML dependency
no modelData[index] usage in LibraryPage
C++ model files under size guard
Qt ASCII mirror build pass
screenshots pass for albums, recent, artists, songs, pins, favorites, videos, focus
```

## Readiness Verdict

```text
KIVO_QT_P2_MODEL_WIRING_GATE_ACTIVE_001
QML_MUST_CONSUME_CPP_MODEL_ROLES_BEFORE_SQLITE_OR_NETWORK_SOURCES
```
