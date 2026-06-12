# P0-P Commercial Claim Review - 2026-06-12

| Candidate claim | Decision | Evidence rule |
|---|---|---|
| Windows x64 C++ audio core | ALLOWED | Release build and manifest pass |
| Local-file FFmpeg decode | ALLOWED | Existing decode and end-to-end tests |
| Event-driven WASAPI shared output | ALLOWED | Existing hardware smoke |
| WASAPI exclusive negotiation | ALLOWED WITH DEVICE QUALIFIER | Never imply universal device support |
| Bit-perfect capable | ALLOWED WITH STRICT QUALIFIER | Claim only when runtime truth reports active |
| Gapless capable | ALLOWED WITH FORMAT/FILE QUALIFIER | Existing sample-domain tests |
| ReplayGain and software volume | ALLOWED | Must disclose processing disables bit-perfect |
| Reproducible binary | PENDING | Requires two-build byte identity |
| Signed release | FORBIDDEN | No certificate evidence |
| Commercially approved release | FORBIDDEN | External gates remain open |
| Supports every Windows audio device | FORBIDDEN | Matrix is incomplete |
| Ten-hour stability certified | FORBIDDEN | Endurance row is open |
| Hi-Res, DSD, or DoP support | FORBIDDEN | P0-Q research only |
| mpv/libmpv compatibility | FORBIDDEN | P0-R research only |

Marketing text must preserve every qualifier. Capability availability and
runtime activation are different claims.
