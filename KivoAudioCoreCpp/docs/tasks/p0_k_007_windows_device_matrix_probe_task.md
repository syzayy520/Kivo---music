# P0-K-007 Windows Device Matrix Probe

**Task:** KIVO-CPP-AUDIO-CORE-P0-K-007-WINDOWS-DEVICE-MATRIX-PROBE
**Status:** COMPLETED
**Execution Basis:** P0-K physical device evidence and genealogy iron rule
**Last Updated:** 2026-06-12

## Goal

Provide an executable, non-fabricated inventory of active Windows render
endpoints before physical device scenarios are classified as passed.

## Family Tree

```text
tests/hardware/device_matrix/
  fixture/
    device_matrix_record.hpp
  platform/windows/
    apartment/
      com_apartment.hpp
      com_apartment.cpp
    endpoint/
      endpoint_inventory.hpp
      endpoint_inventory.cpp
      endpoint_capability_probe.hpp
      endpoint_capability_probe.cpp
      endpoint_metadata_probe.hpp
      endpoint_metadata_probe.cpp
    driver/
      pnp_device_probe.hpp
      pnp_device_probe.cpp
  runner/
    wasapi_device_matrix_main.cpp
  scenario/
    device_matrix_report.hpp
    device_matrix_report.cpp
    device_matrix_validation.hpp
    device_matrix_validation.cpp
```

## Responsibility Gate

- Fixture files contain records only.
- Apartment files own COM thread initialization only.
- Inventory files enumerate all render endpoint states and resolve default
  roles.
- Metadata files inspect identity, state, properties, and form factor.
- Capability files inspect active endpoint format support and device periods.
- Driver files correlate the endpoint to its parent PnP driver.
- Validation files enforce evidence invariants.
- Report files serialize evidence without changing probe results.
- Runner only coordinates inventory, report, and exit status.

## Evidence Contract

Each active endpoint records:

- Sanitized stable endpoint identity.
- Friendly name and form factor.
- Default console, multimedia, and communications roles.
- Shared mix format.
- Shared and exclusive support for the reported mix format.
- Default and minimum device periods.
- Driver version when exposed by the endpoint property store.

The executable fails if no active endpoint exists, COM or endpoint probing
fails, required format evidence is invalid, shared mix support is absent, or no
default render role is represented.

Missing device categories do not become false failures or false passes. They
remain explicit physical-lab gaps.

## Completion Evidence

- `kivo_wasapi_device_matrix` builds with `/W4 /WX`.
- Current machine inventory gate passes with 12 endpoint records.
- One active speaker endpoint is reported at 48 kHz, 2 channels, 32 bits.
- Inactive display-audio, headphone, and auxiliary endpoint states remain
  visible instead of disappearing from the matrix.
- The active speaker endpoint correlates to Microsoft driver
  `10.0.22621.2506`; inactive Intel display audio correlates to
  `10.27.0.12`.
- Shared mix support is present; exclusive support for the shared mix format is
  absent and reported truthfully.
- Console, multimedia, and communications defaults resolve to the same active
  endpoint.
- Forbidden-token and genealogy gates authorize only the
  `platform/windows/` branch for Windows implementation tokens.
- Largest family implementation file is 213 lines.
- Physical USB DAC, Bluetooth, active HDMI/DisplayPort, hot-plug,
  default-change, and sleep/resume evidence remains open.
