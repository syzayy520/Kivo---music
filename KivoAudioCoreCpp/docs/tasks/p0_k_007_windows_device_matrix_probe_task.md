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
      endpoint_probe.hpp
      endpoint_probe.cpp
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
- Inventory files enumerate active render endpoints and resolve default roles.
- Probe files inspect one endpoint's identity, properties, format support, and
  device periods.
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
- Current machine inventory gate passes.
- One active speaker endpoint is reported at 48 kHz, 2 channels, 32 bits.
- Shared mix support is present; exclusive support for the shared mix format is
  absent and reported truthfully.
- Console, multimedia, and communications defaults resolve to the same active
  endpoint.
- Forbidden-token and genealogy gates authorize only the
  `platform/windows/` branch for Windows implementation tokens.
- Largest family implementation file is 213 lines.
- Physical USB DAC, Bluetooth, active HDMI/DisplayPort, hot-plug,
  default-change, and sleep/resume evidence remains open.
