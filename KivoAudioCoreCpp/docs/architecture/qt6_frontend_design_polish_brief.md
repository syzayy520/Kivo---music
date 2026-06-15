# Kivo Music Qt6 Frontend Design Polish Brief

**Document ID:** KIVO-QT6-FRONTEND-DESIGN-POLISH-001
**Status:** ACTIVE DESIGN REVIEW BASIS
**Effective Date:** 2026-06-13
**Boundary:** Product frontend design only. No Qt or QML implementation belongs
in `KivoAudioCoreCpp/`.

## Design Goal

Kivo Music should feel like a premium native desktop music player: calm,
precise, fast to scan, and trustworthy about audio truth. The design should
make common playback work effortless while keeping engineering evidence and
commercial claims honest.

## Purpose

Provide the design review basis for the Qt 6 product frontend while preserving
the AudioCore boundary. This brief guides product UI polish without turning the
audio engine repository into a frontend implementation owner.

## Scope

This brief covers product-facing visual hierarchy, component responsibility,
truth-label discipline, accessibility, and screenshot review criteria. It does
not authorize Qt/QML files, product assets, or product build targets inside
`KivoAudioCoreCpp/`.

## Current Phase

The current phase is design and product-shell polish outside AudioCore.
AudioCore may expose evidence-backed contracts, but incomplete evidence must
remain hidden or marked unknown in the frontend.

## Allowed

- Use this brief to judge Qt/QML product screenshots.
- Keep architecture notes about UI truth and boundary responsibilities here.
- Require product-side view models to adapt AudioCore snapshots before QML.
- Keep component ownership narrow and one responsibility per QML file.

## Forbidden

- Do not place QML, artwork, product navigation, frontend settings, or Qt app
  lifecycle code inside `KivoAudioCoreCpp/`.
- Do not show marketing truth labels that AudioCore has not proven.
- Do not mix playback control state with audio-quality truth state.
- Do not build broad catch-all UI service folders.

## Gate

Design acceptance requires screenshot inspection plus product frontend build
checks. AudioCore acceptance still requires the core gate runner and build
validation to pass with no frontend ownership leakage.

## Failure

If a screenshot looks polished but cannot be traced to stable component
ownership, accessible states, and evidence-backed truth labels, it is not a
commercial-quality pass. If a truth label lacks AudioCore evidence, it must be
hidden or shown as unknown.

## Future Phase

Future product phases may add dark mode, global media controls, tray
integration, device pickers, lyrics, library persistence, and packaging after
their product-side ownership and AudioCore evidence are designed and tested.

## Deferred

Mobile UI, NAS/WebDAV browsing, cloud drive login, AI curation, commercial
feedback systems, and installer/signing workflows are deferred to separate
product tickets.

## Last Updated

2026-06-15 by Codex during blocker recovery audit.

## Product Personality

- Native desktop, not web dashboard.
- Music-first, not settings-first.
- Quietly technical, not noisy or decorative.
- Evidence-backed, not marketing-led.
- Dense enough for library work, spacious enough for now-playing focus.

## Primary Screens

```text
Now Playing
  owns current track focus, artwork, transport context, audio truth summary

Library
  owns browsing, filtering, sorting, and local metadata display

Queue
  owns upcoming playback order, drag/reorder, and transition visibility

Devices
  owns output endpoint selection, supported formats, active mode, failures

Diagnostics
  owns user-visible health plus separate engineering evidence drilldown

Settings
  owns product preferences only, never AudioCore internals
```

## First-Viewport Layout

Desktop default:

```text
left navigation rail
main content surface
right optional queue/detail panel
bottom transport bar
```

Rules:

- The bottom transport bar has fixed height and never jumps when metadata,
  device names, or status labels change.
- The active screen title, primary action, and current playback state must be
  visible without opening a menu.
- The now-playing surface should show a hint of queue or library context, not
  isolate playback from navigation.
- Avoid nested cards. Use full-width bands, separators, density changes, and
  spacing before adding frames.

## Visual System

Core tokens:

```text
spacing: 4, 8, 12, 16, 24, 32
radius: 4 for controls, 6 for panels, 8 maximum for repeated cards
icon sizes: 16, 20, 24, 32
transport control hit area: 40 minimum
row heights: 32 compact, 40 standard, 56 rich
```

Palette direction:

- Use a neutral graphite base with warm metal, soft white, and one controlled
  accent.
- Do not let the UI become all-purple, all-blue, all-beige, or all-slate.
- Album art may provide local color, but controls and truth labels must remain
  legible without relying on artwork colors.
- Reserve warning/error colors for actual device, decode, source, or evidence
  problems.

Typography:

- Track title: prominent but not hero-scale except in a dedicated now-playing
  focus mode.
- Artist, album, queue, and device labels: compact and stable.
- Truth labels: short, aligned, and never animated.
- Long names: truncate with tooltip or detail view; do not squeeze controls.

## Playback Controls

Required controls:

```text
previous
play/pause
next
seek timeline
elapsed/remaining time
volume
output device
play mode
queue visibility
```

Rules:

- Use familiar icons for transport actions.
- Keep control dimensions stable across hover, pressed, loading, disabled, and
  error states.
- Separate transport state from audio truth state. A playing icon is not proof
  that output is bit-perfect or exclusive.
- Timeline drag must show intended seek target without claiming seek completion
  before AudioCore confirms it.

## Audio Truth UI

Truth badges must be evidence-backed:

```text
sample rate
bit depth
container bits
exclusive/shared
engine in path
conversion active
processing active
device format match
```

Rules:

- Show "Unknown" or hide advanced truth when evidence is incomplete.
- Never show Hi-Res, DSD, DoP, bit-perfect, or gapless as a marketing badge
  unless the corresponding AudioCore evidence proves it.
- If conversion is active, say converted PCM rather than native DSD or DoP.
- Device support and active output format need separate visual slots.

## Qt6 Component Family

Future QML component ownership:

```text
ui/qml/components/navigation/
  NavigationRail.qml
  NavigationItem.qml

ui/qml/components/transport/
  TransportBar.qml
  TransportButton.qml
  SeekTimeline.qml
  VolumeControl.qml

ui/qml/components/truth/
  AudioTruthStrip.qml
  TruthBadge.qml
  DeviceTruthRow.qml

ui/qml/components/library/
  TrackTable.qml
  TrackRow.qml
  LibraryFilterBar.qml

ui/qml/components/queue/
  QueuePanel.qml
  QueueRow.qml

ui/qml/components/diagnostics/
  HealthSummary.qml
  EvidenceRow.qml
```

Each component owns one visual responsibility. View models own data adaptation.
No QML component calls the Host ABI directly.

## Design Review Checklist

Use this checklist when the initial design images are available:

- Navigation: current location is obvious at a glance.
- Transport: primary playback controls are centered, stable, and reachable.
- Hierarchy: artwork, metadata, queue, and controls do not fight for the same
  visual weight.
- Density: library and queue rows support long metadata without overlap.
- Truth: technical labels are close to the thing they describe.
- Devices: active device, selected device, supported formats, and failures are
  not conflated.
- Empty states: missing library, missing device, failed open, and unsupported
  format each have distinct states.
- Accessibility: keyboard focus, contrast, hit targets, and non-color status
  indicators are present.
- Motion: animations clarify transitions and never obscure critical status.
- Claims: no screen implies commercial support that AudioCore evidence has not
  proven.

## Required Design Assets

Please keep design references in the product repository under:

```text
packages/design/references/
  raw/
  reviewed/
  notes/
```

For this AudioCore repository, only design decisions and boundary notes are
tracked. The actual image sources should not be committed here unless they are
architecture evidence rather than product UI assets.

## Readiness Verdict

```text
KIVO-QT6-FRONTEND-DESIGN-POLISH-READY-001
DESIGN_REVIEW_BASIS_READY_WAITING_FOR_USER_DESIGN_IMAGES
```
