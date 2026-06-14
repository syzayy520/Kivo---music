# KivoMusicQt Family Gate

**Document ID:** KIVO-QT-FAMILY-GATE-001
**Status:** ACTIVE STRUCTURE CONTROL
**Effective Date:** 2026-06-13

## Required Loop

Every UI slice must follow this order:

```text
planning design
natural family tree
implementation
detail polish
effect inspection
static structure self-check
local validation
real screenshot inspection
next polish loop
```

No slice is complete just because it renders once. A screen is complete only
when its structure, visual states, data boundary, real screenshot, and
validation evidence are all acceptable.

## Family Rules

- Prefer a subfolder before adding a file to a broad folder.
- One QML file owns one smallest visual responsibility.
- One C++ class owns one model, service, bridge, cache, or mapping role.
- Pages and windows compose feature/component owners only.
- View models adapt state for QML; QML does not call native services.
- Product services do not import QML.
- AudioCore integration is isolated under `native/audio_core_bridge/`.
- Media library, artwork, lyrics, settings, diagnostics, and devices are
  separate families.

## Forbidden Shapes

- No direct files under `ui/qml/components/` once a subfamily exists.
- No `services/` file that mixes library, artwork, settings, lyrics, and audio.
- No screen file with playback logic, database logic, native bridge calls, or
  image cache logic.
- No QML component that both owns layout shell and owns business state.
- No UI claim that is stronger than the available AudioCore/product evidence.

## Size Guard

Soft limits for review:

```text
QML component: 180 lines
QML window/page: 260 lines
C++ header: 180 lines
C++ implementation: 260 lines
Markdown architecture note: 260 lines
```

Exceeding a limit is not automatically forbidden, but it requires an explicit
reason or a split plan in the slice acceptance notes.

## First Families

```text
app/
  main.cpp

ui/qml/tokens/
  Theme.qml

ui/qml/windows/
  HomeWindow.qml

ui/qml/components/navigation/
ui/qml/components/shell/
ui/qml/components/content/
ui/qml/components/artwork/
ui/qml/components/transport/
ui/qml/components/truth/

docs/architecture/
docs/design/
tools/gates/
tools/validation/
```

## Readiness Verdict

```text
KIVO-QT-FAMILY-GATE-ACTIVE-001
STRUCTURE_MUST_PASS_BEFORE_UI_SLICE_ACCEPTANCE
```
