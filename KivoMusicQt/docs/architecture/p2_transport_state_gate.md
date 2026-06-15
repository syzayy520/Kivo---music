# P2 Transport State Gate

**Document ID:** KIVO-QT-P2-TRANSPORT-STATE-GATE-001
**Status:** ACTIVE IMPLEMENTATION CONTROL
**Effective Date:** 2026-06-14

## Commercial Intent

The player bar must be driven by a single native state boundary before real
audio, queue, Hi-Res, DSD, DoP, or exclusive-mode work is wired to UI. QML may
compose the transport surface, but it must not own playback truth.

## Natural Family Tree

```text
native/models/transport/
  transport_state.h/.cpp
    current display state for track, progress, format, volume, and play mode

ui/qml/components/transport/
  TransportBar.qml
    layout and state distribution only
  TransportTrackInfo.qml
    current track identity only
  PlaybackControlCluster.qml
    playback controls and progress display only
  AudioStatusText.qml
    format/status text only
  VolumeCluster.qml
    volume display only
```

## Rules

- Future AudioCore updates `TransportState`; QML does not call AudioCore.
- Future queue selection updates `TransportState`; the player bar does not own queue.
- Format text is restrained and factual, not a badge pile.
- The state object is seeded only until real playback wiring exists.

## Future Replacement Path

```text
seeded TransportState
  -> queue-selected track state
  -> AudioCore playback progress and format state
  -> exclusive output / DSD / DoP truth state
```

## Acceptance Evidence

```text
family gate pass
transport hard-coded display text removed from child components
Qt ASCII mirror build pass
Home/Search/Library screenshots show stable player bar
```
