# P0-P Release Matrix - 2026-06-12

| Area | Evidence | Status |
|---|---|---|
| Release build | Clean MSVC x64 Release build and CTest | PENDING PIPELINE |
| Repository gates | Aggregated gate suite | PENDING PIPELINE |
| ABI exports | Exactly seven locked C exports | PENDING PIPELINE |
| PE hardening | ASLR, NX, CFG, CET | PENDING PIPELINE |
| DLL reproducibility | Two clean builds, identical SHA-256 | PENDING PIPELINE |
| Archive reproducibility | Two archive creations, identical SHA-256 | PENDING PIPELINE |
| Symbols | PDB separated from runtime | PENDING PIPELINE |
| Dependency record | FFmpeg version, config, hashes, license | PENDING PIPELINE |
| SBOM | CycloneDX 1.6 | PENDING PIPELINE |
| Manifest privacy | No checkout or Windows user path | PENDING PIPELINE |
| Authenticode | Kivo DLL and product installer signatures | BLOCKED: CERTIFICATE/PRODUCT |
| Corresponding source | Exact FFmpeg source and build-script archive | BLOCKED: EXTERNAL RECORD |
| Legal approval | LGPL, codecs, target markets | BLOCKED: LEGAL REVIEW |
| Installer clean install | Signed product installer consumes payload | BLOCKED: PRODUCT INSTALLER |
| Installer upgrade | Atomic full-payload replacement | BLOCKED: PRODUCT INSTALLER |
| Installer rollback | Prior complete payload restored | BLOCKED: PRODUCT INSTALLER |
| Built-in output | Existing hardware smoke evidence | PASS |
| Callback host ABI path | Source callback to FFmpeg to WASAPI | PASS |
| USB DAC | Physical release-lab row | OPEN |
| Bluetooth | Physical release-lab row | OPEN |
| Active HDMI | Physical release-lab row | OPEN |
| Device unplug/change/sleep | Physical release-lab rows | OPEN |
| Ten-hour endurance | Physical release-lab run | OPEN |

An `OPEN` or `BLOCKED` row cannot be rewritten as `PASS` without the named
artifact or laboratory record.
