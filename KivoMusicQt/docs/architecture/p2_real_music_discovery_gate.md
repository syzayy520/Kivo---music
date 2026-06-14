# P2 Real Music Discovery Gate

**Document ID:** KIVO-QT-P2-REAL-MUSIC-DISCOVERY-GATE-001
**Status:** ACTIVE IMPLEMENTATION CONTROL
**Effective Date:** 2026-06-14

## Commercial Intent

Kivo Desktop must move beyond visual seed data by discovering real local and
NAS music files. This slice creates a real source boundary without claiming
playback, metadata completeness, artwork extraction, or database indexing.

## Natural Family Tree

```text
native/sources/music/
  music_source_root.h
    one source root value
  music_source_roots.h/.cpp
    default local and NAS root selection
  audio_file_type.h/.cpp
    supported file suffix and display format classification
  music_filename_parser.h/.cpp
    filename-only title/artist extraction
  music_file_record.h
    neutral discovered file record
  music_file_scanner.h/.cpp
    bounded recursive file discovery

native/models/library/
  music_library_projection.h/.cpp
    discovered records to library model rows

native/models/search/
  music_search_projection.h/.cpp
    discovered records to search result rows
```

## Rules

- QML must not read local paths, NAS paths, or file metadata.
- Source scanning is capped during this slice to protect app startup.
- Filename parsing is a temporary display strategy, not a metadata engine.
- DSD file discovery is classification only, not playback support.
- Empty or inaccessible sources may fall back to demo seed data.

## Future Replacement Path

```text
bounded startup scan
  -> background scanner
  -> SQLite catalog
  -> metadata parser and artwork cache
  -> NAS/WebDAV/cloud source adapters
```

## Acceptance Evidence

```text
real local Music path visible to the scanner
real NAS Music path visible to the scanner
family gate pass
QML contains no file-system source access
Qt ASCII mirror build pass
Library/Search screenshots show real discovered songs
```
