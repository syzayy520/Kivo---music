# P2 Search Model Wiring Gate

**Document ID:** KIVO-QT-P2-SEARCH-MODEL-WIRING-GATE-001
**Status:** ACTIVE IMPLEMENTATION CONTROL
**Effective Date:** 2026-06-14

## Commercial Intent

Search must not become a QML demo island. The page is a visual composition
surface; result ownership, filter ownership, future indexing, and future source
ranking belong to C++ model families.

## Natural Family Tree

```text
native/models/search/
  search_result_model.h/.cpp
    row roles for title, detail, category, artwork variant
  search_filter_model.h/.cpp
    row roles for available filter labels
  search_model_bundle.h/.cpp
    QML-facing composition root only
  demo_search_seed.h/.cpp
    temporary seed data, removable when indexing is ready

ui/qml/pages/search/
  SearchPage.qml
    layout and model binding only
```

## Rules

- QML consumes `searchModels.results` and `searchModels.filters`.
- QML may not own search arrays or index into `modelData`.
- The bundle must not query SQLite, scan files, or reach network sources.
- Result role names are stable for local index, NAS/WebDAV, cloud, and AI ranks.

## Future Replacement Path

```text
demo_search_seed
  -> local SQLite FTS index
  -> source-aware ranking
  -> NAS/WebDAV/cloud adapters
  -> AI semantic expansion
```

## Acceptance Evidence

```text
family gate pass
no SearchPage modelData[index] usage
no QML-owned search result/filter arrays
C++ search model files under size guard
Qt ASCII mirror build pass
Search screenshot pass
```
