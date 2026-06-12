# P0-K Windows Device Matrix Evidence - 2026-06-12

## Evidence Rules

- Only devices observed by Windows on this machine are recorded.
- A present driver is not treated as an active render endpoint.
- An unconnected device category remains `NOT PRESENT`, not `PASS`.
- A scenario that was not physically performed remains `NOT EXECUTED`.
- Endpoint IDs are represented by stable one-way hashes.

## WASAPI Inventory

`kivo_wasapi_device_matrix.exe` returned exit code `0`.

| Identity | Endpoint | Category | State | Driver | Mix format | Default roles |
|---|---|---|---|---|---|---|
| `0x621A89954F483265` | Speakers (High Definition Audio Device) | integrated | active | Microsoft `10.0.22621.2506` | 48,000 Hz, 2 ch, 32 bit | console, multimedia, communications |
| multiple sanitized identities | HDMI / display audio | display-audio | not-present | Intel `10.27.0.12` where correlated | unavailable | none |
| sanitized identity | Headphones | unknown | unplugged | unavailable | unavailable | none |

The executable reported 12 total endpoint records. Only the active endpoint
was queried for mix format and shared/exclusive capabilities.

Inventory gate: `PASS`.

## Driver Correlation

The endpoint property store did not expose a usable parent-driver version.
The executable now correlates each MMDevice endpoint to its parent PnP device
through SetupAPI:

| Device | Provider | Driver version | INF | Endpoint state |
|---|---|---|---|---|
| High Definition Audio Device | Microsoft | `10.0.22621.2506` | `hdaudio.inf` | active speaker endpoint |
| Intel Display Audio | Intel Corporation | `10.27.0.12` | `oem39.inf` | driver present, no active endpoint |

The Intel display driver must not be counted as HDMI/DisplayPort playback
evidence until an active endpoint appears and playback is run.

## Physical Matrix

| Requirement | Status | Evidence |
|---|---|---|
| Integrated/onboard audio | PASS | Active endpoint inventory, generated-tone playback, FFmpeg-to-WASAPI playback, and one-hour endurance |
| USB DAC | NOT PRESENT | No active USB render endpoint observed |
| Bluetooth | NOT PRESENT | No active Bluetooth render endpoint observed |
| HDMI/DisplayPort | NOT ACTIVE | Intel display-audio driver present; no active render endpoint |
| Default-device change | NOT EXECUTED | Requires a second active render endpoint |
| Device unplug/replug | NOT EXECUTED | Requires removable output hardware |
| Sleep/resume notification path | PASS | Deterministic callback and one-shot renderer invalidation tests |
| Physical sleep/resume | NOT EXECUTED | Real operating-system transition remains a release-lab row |
| Ten-hour endurance | NOT EXECUTED | Release-candidate endurance remains |

## Existing Onboard Playback Evidence

The one-hour real-device run submitted and accepted `172,800,000` frames with:

- Zero worker wait timeouts.
- Zero underruns.
- Zero device invalidations.

The detailed runtime evidence is in
`docs/validation/p0_f_i_audio_runtime_validation_2026_06_12.md`.

## Commands

```powershell
cmake --build .build/p0c-contract-debug --target kivo_wasapi_device_matrix
.build/p0c-contract-debug/kivo_wasapi_device_matrix.exe
Get-CimInstance Win32_PnPSignedDriver
```

## Implementation Verdict

The executable inventory, hostile-media matrix, endpoint-change path, and
power-transition recovery signal are complete and evidence-backed. Hardware
not present on this machine remains mandatory P0-P release-lab evidence and is
not marked as passed.
