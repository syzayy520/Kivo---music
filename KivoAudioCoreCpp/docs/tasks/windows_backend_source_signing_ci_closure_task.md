# Windows Source, Signing, and CI Closure Task

**Document ID:** KIVO-WINDOWS-CLOSURE-SOURCE-SIGNING-CI-001
**Status:** APPROVED FOR IMPLEMENTATION
**Execution Basis:** Windows backend completion program

## Goal

Close the repository-controlled portions of FFmpeg corresponding-source
custody, Authenticode verification, and pull-request continuous integration.
External legal approval, production certificate custody, and release-lab
evidence remain external gates.

## Family Tree

```text
tools/release/compliance/source/
  acquisition/
    acquire_ffmpeg_source.ps1
    acquire_ffmpeg_build_scripts.ps1
  archival/
    archive_ffmpeg_corresponding_source.ps1
  foundation/
    source_bundle_foundation.ps1
  verification/
    verify_ffmpeg_source_bundle.ps1
  testing/
    test_ffmpeg_source_bundle.ps1

tools/release/signing/
  foundation/
    authenticode_foundation.ps1
  operation/
    sign_release_binary.ps1
  verification/
    verify_authenticode_signature.ps1
  testing/
    test_authenticode_verifier.ps1

.github/workflows/
  kivo-audio-core-ci.yml
```

## Ownership

- Acquisition obtains an explicitly requested FFmpeg revision into generated
  output and records the fully resolved commit.
- Archival exports clean FFmpeg and build-script Git trees without `.git`
  metadata and creates a deterministic archive.
- Source verification safely expands the archive and verifies every payload
  path, size, hash, revision, license, and evidence field.
- Signing operation signs the final Kivo DLL only when an externally supplied
  usable certificate is present.
- Authenticode verification independently checks signature status, expected
  certificate identity, code-signing EKU, SHA-256 verification, timestamp, and
  final artifact hash.
- CI runs deterministic repository gates, platform-neutral Windows build and
  CTest, and release-tool self-tests. CI does not hold signing credentials.

## Security Rules

- No private key, PFX, password, token, or certificate secret is accepted as a
  repository file.
- Acquisition rejects non-official remotes unless an explicit test-only switch
  is supplied.
- A requested full commit must resolve exactly; abbreviated revisions are
  recorded only after resolution to a full commit.
- Archive entries with absolute paths, parent traversal, drive prefixes, or
  case-insensitive duplicates are rejected before extraction.
- Source manifests never rely on archive names alone; every source and build
  script file is hashed.
- A supplied source archive is verified before release packaging, copied beside
  the runtime artifacts, and bound into the build manifest and release index by
  SHA-256 and resolved commits.
- Authenticode verification defaults to trusted-chain and timestamp-required.
  Test allowances are explicit and are not used by release scripts.
- A self-signed test certificate is temporary, non-exportable, and removed at
  the end of the verifier self-test.

## Required Tests

- Local fixture repository can be acquired at an exact full commit.
- Clean FFmpeg and build-script fixture trees archive deterministically.
- Valid source archive passes complete inventory verification.
- Modified archive bytes are rejected.
- Self-signed SHA-256 Authenticode fixture is recognized under explicit test
  allowances.
- A byte-appended signed fixture is rejected as a hash mismatch.
- Existing installer transaction self-test remains in CI.
- Repository gates, Windows build, CTest, and diff hygiene remain green.

## Completion Truth

Repository completion means the tools and CI prove their own contracts.
It does not mean:

- production signing has occurred;
- a timestamp authority was successfully contacted for final release bytes;
- legal counsel approved distribution;
- the exact production FFmpeg archive is in long-term external custody;
- hardware, installer VM, endurance, or user-feedback gates passed.

## Exit

```text
SOURCE_BUNDLE_TOOLCHAIN_SELFTEST_PASS
AUTHENTICODE_VERIFIER_SELFTEST_PASS
WINDOWS_CI_CONFIGURED
COMMERCIAL_RELEASE_EXTERNAL_EVIDENCE_STILL_REQUIRED
```
