# LyricsCore Boundary

**Document ID:** KIVO-LYRICS-CORE-BOUNDARY-001
**Status:** ARCHITECTURE LOCK
**Last Updated:** 2026-06-13

## Purpose

Keep lyrics parsing, providers, caching and timeline selection outside the
realtime audio core while defining the minimum product integration contract.

## Current Phase

Architecture and ownership are locked. `KivoLyricsCoreCpp` has not yet been
created or implemented.

## Decision

Lyrics is not an audio decode, render, processing, device, or realtime
responsibility. It must be implemented as an independent `LyricsCore` module,
owned outside `KivoAudioCoreCpp`.

Recommended sibling project:

```text
Kivo Music/
  KivoAudioCoreCpp/
  KivoLyricsCoreCpp/
```

The product orchestration layer may consume both modules. Neither module may
include the other's private headers or call into the other's worker threads.

## Responsibility Tree

```text
KivoLyricsCoreCpp/
  include/kivo/lyrics/
    document/        # immutable lyric document and metadata
    timeline/        # timestamp, line, word and translation timing
    parser/          # parser boundary and parse result contracts
    provider/        # provider-neutral request/result contracts
    cache/           # cache identity, freshness and storage contracts
    alignment/       # offset and synchronization decisions
    diagnostic/      # privacy-safe provider/parser/cache facts
  src/
    parser/
      lrc/
      enhanced_lrc/
      sylt/
      plain_text/
    provider/
      local/
      remote/
    cache/
    alignment/
  tests/
    contracts/
    parser/
    provider/
    cache/
    alignment/
```

Families grow only when they acquire a distinct owner. Provider networking,
credential handling and cache persistence must not be placed in parser or
timeline files.

## AudioCore Integration Contract

AudioCore exposes playback facts only:

- active session generation
- rendered position in frames
- accepted output sample rate
- seek generation
- paused/playing/stopped state
- discontinuity and source replacement events

LyricsCore converts a product-provided position snapshot to lyric timeline
selection. It never reads WASAPI clocks, FFmpeg frames, render buffers, device
identifiers or user media bytes directly.

The product orchestration layer owns:

1. Track identity mapping.
2. Calling LyricsCore load/search/cache operations off realtime threads.
3. Supplying AudioCore position snapshots.
4. Applying user lyric offset.
5. Rendering current/next lines in UI.
6. Consent, provider credentials, rate limits and privacy policy.

## Required Contracts

- `LyricsDocument`: source identity, language, translation/romanization links,
  timing precision and immutable line collection.
- `LyricsTimelineEntry`: start/end time, text, optional word timings.
- `LyricsLoadResult`: exact, cached, fallback, not found, rejected or failed.
- `LyricsAlignment`: media duration evidence, global offset and confidence.
- `LyricsProviderPolicy`: opt-in network access, timeout, retry and redaction.
- `LyricsCachePolicy`: key, TTL, provenance, invalidation and size limits.

## Forbidden Coupling

- Lyrics parsing or network requests on decode/render/WASAPI threads.
- Lyrics text, file paths, URLs, account ids or credentials in AudioCore
  diagnostics.
- AudioCore linking HTTP, provider SDK, database or lyric parser libraries.
- Treating approximate lyric alignment as sample-accurate playback truth.
- Storing provider secrets in source control or release manifests.

## Validation

- deterministic parser corpus, including malformed and hostile input
- Unicode, BOM, line ending and large-file coverage
- timestamp ordering, overlap, offset and seek tests
- cache corruption, expiry and provider outage tests
- privacy review for diagnostics and provider requests
- product integration tests for pause, seek, track switch and gapless change

LyricsCore can be commercial-grade only after these proofs exist in its own
project. Creating lyric files under AudioCore would violate the scope lock.

## Allowed

- A sibling LyricsCore project with platform-neutral contracts.
- Product-side composition using AudioCore position snapshots.
- Offline parsing, provider access, caching and alignment.

## Gate

Implementation starts only in the owning sibling project with parser corpus,
privacy policy, provider policy and product orchestration tests.

## Failure

The boundary fails if lyrics code enters AudioCore, touches realtime threads,
or exports personal text/identifiers through audio diagnostics.

## Future Phase

Create the LyricsCore skeleton, then implement local parsers, deterministic
timeline selection, cache policy and opt-in remote providers in that order.

## Deferred

Provider selection, credentials, product UI, translation services and cloud
sync are deferred to product ownership.
