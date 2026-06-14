# Kivo Music Frontend Technology Stack Decision

**Document ID:** KIVO-FRONTEND-TECH-STACK-DECISION-001
**Status:** ACTIVE PRODUCT FRONTEND DIRECTION
**Effective Date:** 2026-06-13
**AudioCore Boundary:** Planning reference only. No frontend code belongs in
`KivoAudioCoreCpp/`.

## Decision

Use Qt 6 for the Kivo Music desktop frontend. The product application is a Qt
6 desktop app that consumes the versioned AudioCore Host ABI from outside
`KivoAudioCoreCpp/`. AudioCore remains a native C++ audio engine and must not
own product UI, media-library state, artwork, lyrics rendering, settings,
credentials, installer UX, or Qt application lifecycle.

## Chosen Stack

```text
Desktop application: Qt 6
UI language: QML with Qt Quick
Control set: Qt Quick Controls
Native language: C++20
Build system: CMake
Audio bridge: Product-side C++ adapter over the AudioCore C Host ABI
Model/view: QAbstractListModel and QAbstractTableModel for large data
Async boundary: Qt signals/slots and worker objects off the UI thread
Rendering style: Custom QML components with design tokens
Settings storage: Product-side settings service, not AudioCore
Testing: Qt Test for C++, qmltestrunner for QML, screenshot regression for key views
Packaging: Qt deployment tooling plus product installer/signing pipeline
```

## Rationale

- Qt 6 gives one native C++ product surface for Windows desktop while keeping
  the AudioCore Host ABI close to the product frontend language boundary.
- QML is the right layer for polished music-player interaction, animation,
  layout, and responsive visual states.
- C++ owns native integration, device-facing product adapters, model classes,
  and the safe wrapper over the AudioCore C ABI.
- Qt Quick Controls provide accessible desktop controls without forcing web
  runtime ownership into the product.
- CMake keeps product frontend build ownership aligned with the existing C++
  engineering environment while remaining outside this AudioCore repository.

## Natural Family Tree

The Qt frontend must live in the product repository, not in
`KivoAudioCoreCpp/`. Preferred product tree:

```text
apps/
  desktop-qt/
    CMakeLists.txt
    app/
      main.cpp              # QApplication/QQmlApplicationEngine bootstrap only
      resources.qrc         # QML and asset registration
    native/
      audio_core_bridge/    # Host ABI loader and safe wrapper
      app_lifecycle/        # startup, shutdown, single-instance behavior
      diagnostics/          # product diagnostics mapping
      settings/             # product settings persistence
    ui/
      qml/
        App.qml             # root composition only
        windows/            # app windows and shell surfaces
        features/
          playback/         # transport, timeline, queue controls
          library/          # browsing and filtering
          now_playing/      # album art, metadata, lyrics surface
          devices/          # output device and format truth UI
          settings/         # preferences
          diagnostics/      # visible health and evidence reports
        components/         # reusable QML controls
        tokens/             # color, spacing, typography, motion values
      assets/
        icons/
        imagery/
        fonts/
    tests/
      cpp/
      qml/
      screenshots/

packages/
  design/
    tokens/
    references/
    review_notes/
  audio_contract/
    snapshots/
    commands/
    diagnostics/
```

## Responsibility Rules

- `App.qml` and window files compose features only. They must not contain
  playback business logic.
- Feature QML files own user interaction surfaces, not AudioCore calls.
- Product C++ view models own state adaptation from AudioCore snapshots to QML.
- `native/audio_core_bridge/` is the only product-side owner of Host ABI
  loading, symbol resolution, handle lifetime, and unsafe/native conversion.
- QML receives immutable or explicitly updated view state. It must not infer
  bit-perfect, Hi-Res, DSD, DoP, gapless, or device truth from UI state.
- LyricsCore remains a separate module. The Qt frontend joins lyrics to
  AudioCore timeline snapshots above realtime/native boundaries.
- `KivoAudioCoreCpp/` owns native audio truth only. It must not import Qt,
  QML, product routes, media-library schema, artwork, lyrics, or settings UI
  concepts.

## Design Direction

Kivo Music should feel like a premium desktop music instrument, not a web
dashboard:

- Persistent left navigation for Library, Now Playing, Queue, Devices,
  Diagnostics, and Settings.
- Full-height now-playing area with clear album art, title, artist, format
  truth, output mode, and timeline.
- Bottom transport bar with stable dimensions for previous, play/pause, next,
  seek, volume, output device, and mode truth.
- Device and quality indicators must use evidence-backed wording. Avoid
  marketing badges until AudioCore evidence proves them.
- Dense library views should use native-feeling table/list models with stable
  row heights and keyboard navigation.
- Diagnostics must separate user-friendly status from engineering evidence.

## Initial Design Polish Checklist

Apply this checklist to the user's initial design images before implementation:

- Layout hierarchy: navigation, content, now-playing, queue, and transport are
  visually distinct without nested decorative cards.
- Playback controls: icon-first, fixed-size, keyboard reachable, and stable
  across hover/pressed/disabled states.
- Audio truth: sample rate, bit depth, output mode, exclusive/shared, engine
  participation, and conversion state are visible only where evidence exists.
- Library density: long titles, artists, paths, and formats truncate or wrap
  deliberately without overlapping.
- Device surface: device name, mode, supported format, active format, and
  failure state have separate visual slots.
- Theme: avoid a one-note purple/blue or dark-slate-only palette; use restrained
  contrast with a real accent system.
- Motion: reserve animation for state transitions, progress, and transport
  feedback; do not animate critical truth labels.
- Accessibility: minimum contrast, focus rings, screen-reader labels for icon
  buttons, and no color-only status.

## Forbidden Shapes

- No Qt, QML, product app, or `CMakeLists.txt` frontend targets inside
  `KivoAudioCoreCpp/`.
- No direct QML access to render, decode, queue, WASAPI, or FFmpeg internals.
- No AudioCore callbacks into QML or UI objects.
- No UI state as the source of truth for native playback state.
- No product claim text derived from incomplete AudioCore evidence.
- No single `services/` folder containing unrelated playback, device, library,
  credential, and diagnostics logic.

## First Implementation Slice

When the product frontend repository is available, create only the Qt shell and
typed boundary skeleton:

```text
apps/desktop-qt/
  app/
  native/audio_core_bridge/
  ui/qml/
    App.qml
    windows/
    features/playback/
    components/
    tokens/
  tests/cpp/
  tests/qml/
packages/design/
packages/audio_contract/
```

The first slice must prove:

- Qt app starts.
- QML shell renders.
- CMake configure/build passes.
- One mock AudioCore snapshot reaches a QML view model.
- No real AudioCore DLL is loaded until Host ABI path, signing, lifecycle, and
  installer ownership are explicitly admitted.

## Validation Commands

Expected product-repository commands after scaffold:

```powershell
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
qmltestrunner
```

AudioCore repository guard while frontend is separate:

```powershell
rg -n "App\\.qml|QApplication|QQmlApplicationEngine|QtQuick|Qt6::Quick|qt_add" .
powershell -ExecutionPolicy Bypass -File .\tools\gates\run_cpp_audio_core_gates.ps1 -ProjectRoot .
```

## External References

- Qt 6 documentation: `https://doc.qt.io/qt-6/`
- Qt Quick: `https://doc.qt.io/qt-6/qtquick-index.html`
- Qt Quick Controls: `https://doc.qt.io/qt-6/qtquickcontrols-index.html`
- Qt for CMake: `https://doc.qt.io/qt-6/cmake-manual.html`
- Qt Test: `https://doc.qt.io/qt-6/qttest-index.html`

## Readiness Verdict

```text
KIVO-FRONTEND-TECH-STACK-LOCKED-001
QT6_QML_CPP_PRODUCT_FRONTEND_SELECTED_AUDIOCORE_REMAINS_UI_FREE
```
