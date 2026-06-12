# P0-J Product State and Recovery Closeout

**Milestone:** P0-J Product State and Recovery
**Status:** STRUCTURAL GATE PENDING
**Closeout Date:** 2026-06-12
**Branch:** `codex/audio-core-execution-roadmap`

## Delivered Families

- Serialized Session ownership, lifecycle, pause, resume, seek, and replacement.
- Decode, seek, flush, render, and device generation isolation.
- EOS drain tickets with completion, timeout, cancellation, and close behavior.
- Device-loss reopen transaction with position restoration.
- Decode/source failure classification with truthful bounded fallback.
- Runtime coordination over decoder, queue, producer, renderer, and Session.

## Required Race Evidence

| Required scenario | Deterministic evidence |
|---|---|
| Continuous seek | `rapid_seek_coalesces_and_restores_state` |
| Rapid next-track commands | `rapid_replacement_keeps_latest_generation` |
| Pause during track switch | `stale_pause_cannot_mutate_replaced_source` |
| Seek during recovery | `recovery_interrupts_seek_without_restoring_stale_intent` |
| Close during drain | `close_waits_for_active_drain_and_releases_runtime` |
| Shutdown during write | `shutdown_waits_for_active_write_and_releases_runtime` |
| Source disconnect during seek | `failed_seek_can_be_closed_and_shutdown_is_terminal` |
| Device loss during flush | `device_loss_during_flush_recovers_pipeline` |
| Old command in new Session | `stale_command_cannot_mutate_renderer` |
| Old block after seek/replacement | `seek_resets_all_scopes_and_discards_old_audio`, `stale_source_generation_fails_before_queue_mutation` |

## Validation Evidence

- MSVC Debug build passes with `/W4 /WX`.
- Public headers compile independently and together.
- Session scenarios pass 19/19.
- Runtime scenarios pass 14/14.
- FakeRenderer scenarios pass 25/25.
- CTest passes 11/11 executables.
- Cumulative audio-core gates pass 9/9.
- Runtime, Session, and Recovery governed roots contain zero flat files.
- Runtime, Session, Recovery, and related test files remain below 260 lines.

## Remaining Structural Gate

- `src/core/playback/pipeline/decode_render_queue_producer.cpp` remains 370
  lines and must be divided into natural Pipeline subfamilies.
- Pipeline public/source/test roots must be added to the genealogy and
  responsibility-size gate before this closeout may be marked `CLOSED`.

## Deferred to P0-K and Later

- Hardware and driver matrix.
- Long-duration endurance and resource-growth evidence.
- Corrupt-media corpus expansion.
- Gapless sample-boundary accuracy.
- Exclusive mode and bit-perfect truth.
- Processing, host ABI, packaging, signing, and release compliance.
