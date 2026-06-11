# FFmpeg LGPL Shared Development Baseline

**Dependency:** FFmpeg
**Purpose:** Local-file demux, audio decode, and explicit PCM conversion
**Integration:** Dynamic linking to replaceable Windows DLLs
**Status:** DEVELOPMENT BASELINE LOCKED
**Last Updated:** 2026-06-12

## Selected Build

- Project: FFmpeg
- Upstream stable branch: 7.1
- Binary version:
  `n7.1.4-39-ga5faeca88f-20260611`
- Library ABI:
  - libavutil 59.39.100
  - libavcodec 61.19.101
  - libavformat 61.7.103
  - libswresample 5.3.100
- Build provider: BtbN/FFmpeg-Builds
- Release publication: 2026-06-11T15:38:21Z
- Asset:
  `ffmpeg-n7.1-latest-win64-lgpl-shared-7.1.zip`
- Asset size: 62,055,065 bytes
- SHA-256:
  `c8bc13fb4079fc477ecca83985d913d43d1a69efdb974ebc1f37490e603b5b79`
- Source URL:
  <https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-n7.1-latest-win64-lgpl-shared-7.1.zip>

The release URL uses the provider's moving `latest` alias. Reproducibility is
therefore anchored to the exact archive hash, embedded FFmpeg git revision,
library ABI versions, publication timestamp, and asset size above.

## License Configuration Evidence

The selected package:

- declares GNU Lesser General Public License version 3;
- is configured with `--enable-shared`;
- is configured with `--disable-static`;
- does not contain `--enable-gpl`;
- does not contain `--enable-nonfree`;
- disables GPL codec integrations including x264, x265, xvid, and rubberband.

FFmpeg's official legal guidance recommends disabling GPL/nonfree components
and using DLL dynamic linking on Windows for the LGPL route:
<https://ffmpeg.org/legal.html>.

## Repository and Build Rules

- FFmpeg headers, DLLs, import libraries, archives, and source are not
  committed to this repository.
- CMake never downloads FFmpeg.
- The local SDK root is supplied through `KIVO_FFMPEG_ROOT`.
- Builds without `KIVO_FFMPEG_ROOT` still compile and test the platform-neutral
  decode boundary, but skip the FFmpeg adapter and hardware playback target.
- Adapter builds validate required headers, import libraries, DLLs, and ABI
  major versions before enabling the target.
- Runtime tests copy only required DLLs into generated build output.

## Commercial Release Obligations

Before any commercial package ships:

- legal review must confirm the selected configuration and all transitively
  enabled libraries;
- the exact corresponding FFmpeg source must be archived and offered;
- FFmpeg copyright and LGPL text must ship;
- users must be able to replace the dynamically linked FFmpeg DLLs;
- build configuration, source revision, binary hashes, and modifications must
  be recorded;
- codec patent obligations must be reviewed separately from copyright license;
- SBOM and third-party notices must include every shipped DLL.

This document records engineering provenance. It is not legal advice and does
not replace release counsel approval.
