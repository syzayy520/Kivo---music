# FFmpeg Corresponding Source Custody Record

**Status:** TEMPLATE - NOT APPROVED

This record must be completed for the exact FFmpeg binary shipped with a
commercial Kivo release.

## Required Evidence

| Field | Required value |
|---|---|
| Kivo release | |
| FFmpeg binary revision | `n7.1.4-39-ga5faeca88f-20260611` |
| FFmpeg source commit | commit beginning `a5faeca88f` |
| Build-scripts commit | |
| Source archive SHA-256 | |
| Source archive storage location | |
| Retention owner | |
| Retention expiry | |
| Public/source-offer delivery route | |
| Legal reviewer | |
| Review date | |
| Approved markets | |
| Codec/patent limitations | |

Generate the technical archive with:

```powershell
powershell -ExecutionPolicy Bypass -File `
  .\tools\release\compliance\archive_ffmpeg_corresponding_source.ps1 `
  -FfmpegSourceRoot <exact-clean-ffmpeg-checkout> `
  -BuildScriptsRoot <exact-clean-build-scripts-checkout>
```

The script verifies a clean exact FFmpeg revision and archives tracked source
plus tracked build scripts deterministically. Its output does not constitute
legal approval or long-term custody by itself.
