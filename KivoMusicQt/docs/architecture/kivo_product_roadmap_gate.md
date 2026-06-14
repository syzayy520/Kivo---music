# Kivo Product Roadmap Gate

**Document ID:** KIVO-PRODUCT-ROADMAP-GATE-001
**Status:** ACTIVE PLANNING CONTROL
**Effective Date:** 2026-06-13

## Planning Verdict

Do not expand NAS, WebDAV, LAN discovery, cloud drives, or AI before the desktop
UI is connected to a small real product data path. A beautiful static shell is
useful for design validation, but a commercial music app becomes trustworthy
only when the UI reads from owned models, shows real library state, and receives
playback truth from a controlled bridge.

The correct next phase after the current UI polish is:

```text
UI excellence
-> C++ model wiring
-> local library and artwork truth
-> AudioCore bridge truth
-> network source families
-> intelligence features
```

## Commercial Reason

- UI polish first prevents backend shape from leaking into screens.
- Model wiring second prevents fake QML arrays from becoming product logic.
- Local library third gives every later source one common media identity.
- Audio truth before network growth prevents false Hi-Res, DSD, DoP, or device claims.
- NAS and WebDAV need the same source/index/artwork/cache contracts as local files.
- AI must operate on indexed metadata and user-approved signals, not raw random paths.

## Phase Order

### P1 UI Production Baseline

Goal: make the desktop shell visually credible and internally structured.

Required evidence:

```text
family gate pass
Qt build pass
screenshot pass for Home, Search, New, Radio, Made, Albums, Artists, Songs,
Pins, Favorites, Videos, and Focus
no large page doing service/model work
no loud false audio claims
```

Exit rule: the app can still use demo data, but every visible page must have a
clear future model boundary and must not feel like an empty admin panel.

### P2 Product Model Wiring

Goal: replace QML demo arrays with C++ model families.

Natural family tree:

```text
native/models/
  library/
    album_model
    artist_model
    track_model
    playlist_model
  playback/
    playback_state_model
    queue_model
  discovery/
    recommendation_model
    station_model
  search/
    search_result_model
```

Rules:

- QML pages consume model roles only.
- Demo data remains isolated or is removed after model parity.
- Every model has stable role names and screenshot fixtures.

### P3 Local Library, Metadata, And Artwork

Goal: make local files real before network sources.

Natural family tree:

```text
native/services/
  library_index/
  metadata_scan/
  artwork_cache/
native/database/
  schema/
  migrations/
native/image_provider/
  artwork/
```

Required capabilities:

- SQLite schema and migration guard.
- folder import and rescan.
- tag scan off the UI thread.
- embedded artwork extraction.
- `image://artwork/...` provider.
- missing artwork fallback.

### P4 AudioCore Bridge And Truth Surface

Goal: connect playback state without polluting UI or AudioCore.

Natural family tree:

```text
native/audio_core_bridge/
  loader/
  command/
  snapshot/
  diagnostic/
native/models/playback/
native/models/devices/
native/models/diagnostics/
```

Rules:

- UI never claims bit-perfect, Hi-Res, DSD, DoP, exclusive, or gapless without
  bridge evidence.
- The transport bar stays music-first; deep truth lives in a detail surface.
- AudioCore remains Qt-free.

### P5 Network Source Families

Goal: add sources only after local identity and indexing are stable.

Natural family tree:

```text
native/sources/
  local/
  nas/
    smb/
    nfs/
  webdav/
  lan_discovery/
    mdns/
    ssdp/
  cloud_drive/
    provider_contract/
    oauth/
    sync_cursor/
```

Priority:

1. NAS read-only library source.
2. LAN discovery for finding NAS/media servers.
3. WebDAV read-only source.
4. Cloud drive provider contract.
5. Offline cache and conflict policy.

Rules:

- Every source maps into the same media identity model.
- Credentials are never stored in QML.
- Network scans are cancellable, resumable, and rate-limited.
- Failed sources have designed empty/error states.

### P6 AI Features

Goal: intelligence that strengthens the music product, not decoration.

Allowed first features:

```text
smart search over indexed metadata
playlist explanation and refinement
library cleanup suggestions
duplicate detection assistance
listening-context recommendations
lyrics/metadata enrichment review queue
```

Rules:

- AI never mutates the library without user review.
- AI never reads credentials.
- AI outputs are provenance-tagged.
- Local/private library privacy policy is a product feature, not a footnote.

## Stop Conditions

Pause feature growth when any of these are true:

- QML starts owning product business state.
- A new feature bypasses existing source/model/database families.
- UI makes stronger audio claims than available evidence.
- A source requires credentials before the credential family exists.
- Screenshots show empty, clipped, or placeholder-looking primary pages.
- Build, family gate, or screenshot capture fails.

## Current Recommendation

After the current UI screenshot loop passes, start P2 Product Model Wiring before
NAS, WebDAV, LAN discovery, cloud drive, or AI. The first implementation slice
should be a read-only C++ library model feeding Albums, Artists, Songs, and
Search while keeping the current QML layout unchanged.

## Readiness Verdict

```text
KIVO_PRODUCT_ROADMAP_GATE_ACTIVE_001
NEXT_AFTER_UI_IS_CPP_MODEL_WIRING_NOT_NETWORK_OR_AI_FIRST
```
