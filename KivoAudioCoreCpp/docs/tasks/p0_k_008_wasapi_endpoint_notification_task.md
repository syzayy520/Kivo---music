# P0-K-008 WASAPI Endpoint Notification

**Task:** KIVO-CPP-AUDIO-CORE-P0-K-008-WASAPI-ENDPOINT-NOTIFICATION
**Status:** COMPLETED
**Execution Basis:** P0-K device-change stress and P0-J recovery policy
**Last Updated:** 2026-06-12

## Goal

Detect default render endpoint replacement and active endpoint loss before the
next WASAPI call happens to return a device-invalidated error.

## Family Tree

```text
src/platform/windows/wasapi/
  device/
    notification/
      default_render_endpoint_observer.hpp
      default_render_endpoint_observer.cpp
  renderer/
    wasapi_renderer_notification.cpp
```

## Responsibility Gate

- The observer implements `IMMNotificationClient` only.
- The observer callback path uses fixed storage and atomics only.
- Callbacks do not allocate, wait, log, reopen devices, or run state policy.
- The renderer notification file consumes one pending change and maps it to
  renderer state, diagnostics, and device generation.
- Open registers the observer after resolving the selected endpoint.
- Close unregisters before releasing the enumerator and observer.

## Behavior

- A new default `eRender/eConsole` endpoint publishes a change.
- Removing the selected endpoint publishes a change.
- Making the selected endpoint inactive publishes a change.
- Capture, multimedia-role, property, and unrelated endpoint events do not
  invalidate the current console renderer.
- The first render operation after a change reports `DeviceLost`, sets
  `reopen_required`, increments the invalidation count, and advances device
  generation.
- The atomic pending flag is consumed once; duplicate recovery signals are not
  emitted for one observed change.

## Validation Evidence

- WASAPI adapter scenarios: 17 of 17 passed.
- Default replacement, irrelevant role, inactive endpoint, and removal callback
  behavior are deterministic unit scenarios.
- Real shared-mode smoke after observer registration:
  - 144,000 submitted frames.
  - 144,000 worker-accepted frames.
  - Zero worker wait timeouts.
  - Zero underruns.
  - Zero device invalidations.
- Largest WASAPI production source remains 241 lines.

## Physical Gap

The callback and recovery signal path is implemented. A physical switch to a
second active output and an unplug/replug cycle remain required before those
P0-K matrix rows can be marked `PASS`.
