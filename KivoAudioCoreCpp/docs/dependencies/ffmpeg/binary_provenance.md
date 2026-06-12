# FFmpeg Binary Provenance

**Record date:** 2026-06-13
**Technical status:** VERIFIED LOCAL BASELINE
**Legal status:** EXTERNAL REVIEW OPEN

## Binary Archive

| Field | Value |
|---|---|
| Provider | BtbN/FFmpeg-Builds |
| Asset | `ffmpeg-n7.1-latest-win64-lgpl-shared-7.1.zip` |
| Asset URL | `https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-n7.1-latest-win64-lgpl-shared-7.1.zip` |
| Local archive size | `62055065` bytes |
| Local archive SHA-256 | `c8bc13fb4079fc477ecca83985d913d43d1a69efdb974ebc1f37490e603b5b79` |
| Reported binary revision | `n7.1.4-39-ga5faeca88f-20260611` |
| FFmpeg source revision | commit beginning `a5faeca88f` |

The local retained ZIP hash matches the repository-locked baseline. The
installed SDK `include/libavutil/ffversion.h` and `ffmpeg.exe -version` report
the same binary revision.

## Build-Script Provenance

GitHub release metadata observed on 2026-06-13:

| Field | Value |
|---|---|
| Release tag | `latest` |
| Release published | `2026-06-12T15:18:43Z` |
| Asset updated | `2026-06-12T15:18:11Z` |
| Tag target commit | `a9410e4be2b332e535000004a8ebf304d9b46689` |
| Target commit author date | `2026-06-10T17:39:42Z` |
| Build repository | `https://github.com/BtbN/FFmpeg-Builds.git` |

The build-script commit association is an evidence-based inference: the
mutable `latest` release serves the retained asset whose bytes match the
locked local SHA-256, and the release tag resolves to the commit above. The
technical source bundle must retain that exact commit, the exact FFmpeg commit,
and the matching binary archive hash together.

## Acquisition

```powershell
.\tools\release\compliance\source\acquisition\acquire_ffmpeg_source.ps1 `
  -Revision a5faeca88f

.\tools\release\compliance\source\acquisition\acquire_ffmpeg_build_scripts.ps1 `
  -Revision a9410e4be2b332e535000004a8ebf304d9b46689
```

## Limits

This record proves technical provenance available to the repository. It does
not prove legal approval, patent clearance, public source-offer fulfillment,
retention ownership, or long-term custody.
