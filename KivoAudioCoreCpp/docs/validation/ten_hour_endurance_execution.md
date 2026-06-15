# Ten-Hour WASAPI Endurance Execution

**Status:** TOOLING READY - PHYSICAL RUN OPEN

The hardware tone smoke supports one-hour cycles. The endurance wrapper runs
ten consecutive cycles, captures the complete console record, samples process
resources, checks runtime failure counters, hashes every evidence artifact,
and writes a v2 machine-readable report.

```powershell
powershell -ExecutionPolicy Bypass -File `
  .\tools\validation\endurance\run_wasapi_endurance.ps1 `
  -ToneSmokePath .\out\<release-build>\kivo_wasapi_tone_smoke.exe `
  -Hours 10 `
  -Mode shared `
  -DeviceEvidencePath .\out\<device-row>\device-matrix-evidence.json `
  -DeviceRowId integrated-shared
```

Run shared and supported exclusive modes separately on each required physical
device row. The wrapper does not infer device identity; pair its evidence with
the device-matrix output from the same machine and time window.

Passing classification requires:

- process exit code zero
- requested duration at least ten hours
- complete log and hashes retained
- zero underruns, protocol overruns, invalidations or worker wait failures
- device/driver/OS identity attached
- private bytes, handles, threads, GDI and USER object growth within the
  configured thresholds
- resource CSV and matching device evidence hashes retained

A one- to nine-hour run is development evidence only. Tool availability does
not mark the ten-hour release row passed. A ten-hour software run without a
matching physical device record is classified as requiring device review.
