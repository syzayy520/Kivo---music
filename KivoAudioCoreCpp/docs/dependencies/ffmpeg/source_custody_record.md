# FFmpeg Corresponding Source Custody Record

**Status:** TECHNICAL ARCHIVE VERIFIED - EXTERNAL CUSTODY AND LEGAL APPROVAL OPEN

This record must be completed for the exact FFmpeg binary shipped with a
commercial Kivo release.

## Required Evidence

| Field | Required value |
|---|---|
| Kivo release | `1.0.0-rc.1` candidate input |
| FFmpeg binary revision | `n7.1.4-39-ga5faeca88f-20260611` |
| FFmpeg source commit | `a5faeca88fdaaf82a151e21ad69b5250374833e4` |
| Build-scripts commit | `a9410e4be2b332e535000004a8ebf304d9b46689` |
| Source archive | `ffmpeg-a5faeca88f-corresponding-source.zip` |
| Source archive size | `21495956` bytes |
| Source archive SHA-256 | `7c4e280cddea5180b045b7f1449fd47ff82d61b6c4d734f1019dbf6ffef66245` |
| Source file inventory | `8831` verified files |
| Technical verification | PASS on 2026-06-13 |
| Source archive storage location | Local generated evidence only; durable external location required |
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
  .\tools\release\compliance\source\archival\archive_ffmpeg_corresponding_source.ps1 `
  -FfmpegSourceRoot <exact-clean-ffmpeg-checkout> `
  -BuildScriptsRoot <exact-clean-build-scripts-checkout>
```

The script verified clean exact revisions, archived tracked source plus build
scripts deterministically, and passed full path/size/hash inventory
verification. The local generated file is not checked into Git and does not
constitute legal approval or durable long-term custody by itself.
