# P0-P Release Matrix - 2026-06-12

| Area | Evidence | Status |
|---|---|---|
| Release build | Clean MSVC x64 Release build and CTest 18/18 | PASS |
| Repository gates | Aggregated gate suite 9/9 | PASS |
| ABI exports | Exactly seven locked C exports | PASS |
| PE hardening | ASLR, NX, CFG, CET | PASS |
| DLL reproducibility | Two clean builds, identical SHA-256 | PASS |
| Archive reproducibility | Two archive creations, identical SHA-256 | PASS |
| Symbols | PDB separated from runtime | PASS |
| Dependency record | FFmpeg version, config, hashes, license | PASS |
| SBOM | CycloneDX 1.6 | PASS |
| Manifest privacy | No checkout or Windows user path | PASS |
| Production diagnostics | Versioned additive ABI snapshot and old-prefix guard | PASS |
| Installer payload verifier | Clean/upgrade/rollback and tamper self-test | PASS |
| Device evidence schema | UTF-8 inventory and required physical row validator | PASS |
| Endurance evidence tooling | Resource sampling and runtime counter classification | PASS |
| Commercial regression gate | Privacy-safe intake and closure validator | PASS |
| Authenticode | Kivo DLL and product installer signatures | BLOCKED: CERTIFICATE/PRODUCT |
| Corresponding source archive | Exact FFmpeg source and build-script archive, 8,831 files, SHA-256 locked | PASS: TECHNICAL |
| Corresponding source custody | Durable retained storage and delivery route | BLOCKED: EXTERNAL CUSTODY |
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
| Ten-hour endurance | Wrapper ready; physical release-lab run | OPEN |

An `OPEN` or `BLOCKED` row cannot be rewritten as `PASS` without the named
artifact or laboratory record.
