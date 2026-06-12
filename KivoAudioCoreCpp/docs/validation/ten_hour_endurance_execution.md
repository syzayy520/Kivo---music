# Ten-Hour WASAPI Endurance Execution

**Status:** TOOLING READY - PHYSICAL RUN OPEN

The hardware tone smoke supports one-hour cycles. The endurance wrapper runs
ten consecutive cycles, captures the complete console record, hashes the
executable and log, and writes machine-readable evidence.

```powershell
powershell -ExecutionPolicy Bypass -File `
  .\tools\validation\endurance\run_wasapi_endurance.ps1 `
  -ToneSmokePath .\out\<release-build>\kivo_wasapi_tone_smoke.exe `
  -Hours 10 `
  -Mode shared
```

Run shared and supported exclusive modes separately on each required physical
device row. The wrapper does not infer device identity; pair its evidence with
the device-matrix output from the same machine and time window.

Passing classification requires:

- process exit code zero
- requested duration at least ten hours
- complete log and hashes retained
- zero unexplained underruns, invalidations or wait failures
- device/driver/OS identity attached
- resource-growth evidence reviewed

A one- to nine-hour run is development evidence only. Tool availability does
not mark the ten-hour release row passed.
