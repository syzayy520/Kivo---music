# Commercial Release Contract

**Purpose:** Define the binding artifact, compatibility, security, privacy,
compliance, upgrade, and approval rules for Kivo Audio Core releases.
**Scope:** Windows x64 release candidates and commercial payloads produced from
`KivoAudioCoreCpp/`.
**Current Phase:** P0-P release hardening.
**Last Updated:** 2026-06-12.

## 1. Purpose

This contract prevents a working development build from being mistaken for a
commercially approved release. Every claim must be backed by stored,
repeatable evidence.

## 2. Scope

The contract covers the Kivo host ABI DLL, its public C headers, required
FFmpeg shared libraries, notices, SBOM, manifests, private symbols, product
installer handoff, upgrade, rollback, and release classification.

## 3. Current Phase

P0-P owns an engineering release-candidate pipeline. Product installer
implementation, certificate custody, external legal approval, source-archive
custody, and physical release-lab execution remain external release inputs.

## 4. Allowed

- Versioned and reproducible Release builds.
- Dynamically linked, replaceable FFmpeg DLLs from the approved baseline.
- Explicit Authenticode signing by certificate thumbprint.
- Explicit unsigned engineering candidates labeled as commercially blocked.
- Separate runtime and symbol archives.
- Privacy-minimized manifests and numeric diagnostics.
- Additive ABI evolution through size/version negotiation.

## 5. Forbidden

- Shipping a debug build or private PDB in the runtime package.
- Storing signing secrets or private keys in the repository.
- Claiming a signature, legal approval, device result, or endurance result
  without verifiable evidence.
- Mixing DLLs from different releases during upgrade or rollback.
- Hiding local paths, credentials, tokens, or user media identifiers in a
  diagnostic or manifest.
- Statically absorbing FFmpeg into the proprietary DLL.
- Advertising shared mode as bit-perfect by default.

## 6. Version and Compatibility Policy

The DLL file version is `major.minor.patch`.

- Major changes may break ABI and require a migration boundary.
- Minor changes are ABI-compatible additions.
- Patch changes are ABI-compatible corrections.
- The host checks ABI version and structure size before use.
- The export list is locked independently from C++ implementation symbols.
- A release-candidate suffix belongs to package metadata; Windows numeric file
  version remains `1.0.0.0` for the first ABI generation.

## 7. Artifact Contract

The runtime archive contains the Kivo DLL, four FFmpeg DLLs, public ABI
headers, licenses, notices, SBOM, installer contract, and release manifests.
The symbols archive contains the Kivo PDB only. Every file and archive has a
SHA-256 record.

## 8. Signing Contract

Commercial distribution requires an Authenticode-signed Kivo DLL and a signed
product installer. SHA-256 file digest, RFC 3161 timestamping, and post-sign
verification are mandatory. Missing certificate access is a blocker, never a
soft pass.

## 9. Dependency and Notice Contract

The release records FFmpeg binary revision, build configuration, DLL hashes,
license, upstream location, build-script location, and corresponding-source
retention duty. Codec availability is not patent or market authorization.

## 10. Crash and Privacy Contract

The core owns bounded numeric diagnostics only. The product host owns crash
capture, redaction, consent, retention, and upload. User paths, account data,
credentials, tokens, and secret-bearing URLs are forbidden from uploaded or
packaged records.

## 11. Upgrade and Rollback Contract

The host must close all ABI handles before replacement. The installer stages
and verifies a complete payload, activates it atomically, and retains the prior
known-good payload until successful startup. Rollback restores the complete
prior set; mixed Kivo and FFmpeg versions are forbidden.

## 12. Gate

Commercial approval requires:

- Clean build, tests, and repository gates.
- Binary and archive reproducibility.
- Locked exports, version resource, and PE hardening.
- Valid signatures.
- Complete notices, SBOM, and corresponding-source archive.
- Legal approval.
- Product installer install, upgrade, rollback, and signature proof.
- Required device, media, recovery, and endurance matrix completion.
- Commercial claim review with evidence links.

## 13. Failure Classification

- `RELEASE_ENGINEERING_FAILED`: repository-controlled evidence failed.
- `ENGINEERING_RC_READY_COMMERCIAL_RELEASE_BLOCKED`: engineering package
  passed, but one or more external commercial gates remain open.
- `COMMERCIAL_RELEASE_APPROVED`: every engineering and external gate passed.

## 14. Future Phase Ownership

P0-Q owns Hi-Res, DSD, and DoP research. P0-R owns isolated mpv/libmpv
reference research. Neither phase may weaken this release contract or alter
current PCM claims without new evidence.

## 15. Deferred

Store submission, certificate procurement, external legal opinion, exact
source-archive custody, product installer implementation, and broad physical
hardware certification remain outside this repository.
