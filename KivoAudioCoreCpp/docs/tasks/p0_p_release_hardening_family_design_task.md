# P0-P Release Hardening Family Design and Execution Taskbook

**Document ID:** KIVO-AUDIO-CORE-P0-P-TASK-001
**Status:** IMPLEMENTATION AUTHORIZED
**Current Phase:** P0-P commercial release hardening
**Last Updated:** 2026-06-12

## 1. Goal

Produce a verifiable Windows x64 release-candidate payload for the completed
audio core. The payload must be reproducible, versioned, hardened, auditable,
privacy-safe, symbol-separated, and consumable by the product installer.

P0-P completion means the repository can generate an engineering release
candidate and truthfully report every remaining external commercial blocker.
It does not permit fabricated signing, legal, installer, or hardware-lab
evidence.

## 2. Base Gate

Before implementation:

- P0-O host ABI and ownership closeout is committed.
- The seven exported C ABI functions are locked.
- Full build, CTest, repository gates, standalone C headers, and hardware ABI
  smoke have passed.
- FFmpeg is dynamically linked and its accepted binary baseline is recorded.
- The working directory is inside `KivoAudioCoreCpp/`.

Failure to satisfy a base condition stops release packaging work.

## 3. Inventory Gate

The release payload owns only:

- `kivo_audio_core.dll`.
- The four required FFmpeg shared libraries.
- Public host ABI headers.
- FFmpeg license and source information.
- Third-party notices.
- CycloneDX SBOM.
- Installer payload contract.
- Build, signing, reproducibility, and verification manifests.

Private PDB files belong only in the symbols archive. Test executables, build
trees, source files, credentials, certificates, and local absolute paths are
forbidden from the runtime archive.

## 4. Natural Family Tree

```text
src/host/abi/
  export/
    kivo_audio_core.def
  resource/
    kivo_audio_core_version.rc.in

tools/release/
  foundation/
    release_foundation.ps1
  build/
    build_release_candidate.ps1
  artifact/
    stage_release_payload.ps1
  compliance/
    write_release_notices.ps1
    write_cyclonedx_sbom.ps1
  signing/
    sign_release_binary.ps1
  manifest/
    write_build_manifest.ps1
  verification/
    compare_reproducible_binary.ps1
    verify_release_candidate.ps1
  runner/
    run_release_candidate.ps1
```

Each file has one release responsibility. A new responsibility must enter an
existing family when ownership matches; a new subfamily is allowed only when
the responsibility is materially different. Release scripts are subject to
the same 260-line responsibility gate as runtime source.

## 5. Implementation Order

1. Lock semantic version `1.0.0` into the DLL resource.
2. Lock the export surface with a module definition file.
3. Enable MSVC reproducible-build and PE hardening options.
4. Build twice from separate clean Release directories.
5. Require byte-identical Kivo DLL output.
6. Stage the runtime and symbol payloads separately.
7. Generate notices, source information, SBOM, and installer contract.
8. Sign and verify the Kivo DLL when a valid certificate is supplied.
9. Record unsigned status as a blocker when no certificate exists.
10. Generate the build manifest with source, toolchain, dependency, privacy,
    signing, reproducibility, and file-hash records.
11. Verify exports, hashes, PE flags, version resource, symbol separation,
    path privacy, and signature state.
12. Create deterministic runtime and symbol archives.
13. Recreate the runtime archive and require an identical SHA-256.
14. Record the release matrix and commercial claim review.

## 6. ODR and ABI Gate

- The public ABI remains C11-compatible.
- The DLL exports exactly the seven locked `kivo_audio_*` functions.
- No C++, STL, Windows SDK, COM, or FFmpeg type enters an ABI header.
- No release implementation creates a second host ABI declaration.
- Version `1.0.0` means ABI major 1; an incompatible ABI change requires a
  new ABI major and migration contract.
- Structure `size` and `version` negotiation remains the compatibility
  mechanism for additive changes.

## 7. Build and Hardening Gate

Required Release configuration evidence:

- MSVC x64 and C++20.
- Dynamic MSVC runtime.
- `/Brepro`.
- `/ZH:SHA_256`.
- Control Flow Guard.
- Dynamic base relocation.
- NX compatibility.
- CET compatibility where supported by the selected toolchain.
- Version resource present.
- Byte-identical DLLs from two clean build directories.

Any missing required flag or non-reproducible DLL is a source failure, not a
warning.

## 8. Compliance Gate

The package must contain:

- Exact shipped FFmpeg DLL hashes.
- FFmpeg build version and configuration.
- Full LGPL license text.
- Third-party notice.
- Corresponding-source retention instructions.
- CycloneDX 1.6 SBOM.
- Installer payload contract preserving replaceable FFmpeg DLLs.

The exact FFmpeg corresponding-source archive and commercial legal approval
are external release records. Their absence blocks commercial approval but
does not invalidate an honestly labeled engineering candidate.

## 9. Signing and Symbol Gate

- Signing accepts an explicit certificate thumbprint only.
- The certificate must be valid, contain a private key, and support code
  signing.
- SHA-256 digest and RFC 3161 timestamping are mandatory.
- Signature verification must pass after signing.
- An unsigned build requires explicit `-AllowUnsigned` and is labeled blocked.
- Runtime archives contain no PDB.
- Symbols are shipped in a separate hash-addressed archive.

The repository never stores private keys, certificate passwords, or exported
certificate bundles.

## 10. Privacy and Diagnostics Gate

- Manifests contain no source checkout path or Windows user profile path.
- Diagnostic payload descriptions remain numeric ABI fields only.
- Credentials, tokens, URLs containing secrets, user media paths, and account
  identifiers are forbidden from release records.
- Crash upload and user consent belong to the product host, not this core.
- The product host must redact and obtain consent before external upload.

## 11. Upgrade and Rollback Gate

- The product host destroys all ABI handles before DLL replacement.
- Core and FFmpeg DLLs are replaced as one private application payload.
- Rollback restores the previous complete payload, never a mixed DLL set.
- Installer staging must be atomic from the host's perspective.
- The installer verifies archive and file hashes before activation.
- A failed activation retains or restores the prior known-good payload.

## 12. Required Validation

- PowerShell parser validation for every release script.
- Clean Release configure and build.
- Full Release CTest.
- Repository gate suite.
- Responsibility line gate.
- Public C ABI standalone-header compile.
- Exact export audit with `dumpbin`.
- PE hardening audit with `dumpbin`.
- Version-resource audit.
- Two-build binary reproducibility audit.
- Deterministic archive audit.
- Package hash audit.
- Runtime symbol exclusion audit.
- Manifest path-privacy audit.
- Signing verification or explicit signing blocker.
- Device, media, endurance, ABI, upgrade, and rollback matrix review.
- Commercial capability claim review.

## 13. Stop Conditions

Stop commercial approval, but continue engineering evidence collection, when:

- No valid Authenticode certificate is available.
- Exact FFmpeg corresponding source is not archived.
- Legal review is incomplete.
- Product installer signing or atomic upgrade proof is missing.
- Required physical device or endurance rows are open.

Stop the engineering candidate entirely when:

- Build or tests fail.
- Repository gates fail.
- DLLs are not byte-reproducible.
- Export ABI differs.
- A required PE hardening flag is absent.
- Runtime hashes fail.
- Private symbols or local paths leak into the runtime package.
- Release automation hides or downgrades a failed check.

## 14. Completion Classification

Use exactly one classification:

- `ENGINEERING_RC_READY_COMMERCIAL_RELEASE_BLOCKED`
- `COMMERCIAL_RELEASE_APPROVED`
- `RELEASE_ENGINEERING_FAILED`

`COMMERCIAL_RELEASE_APPROVED` is forbidden until signing, legal,
corresponding-source, installer, and required physical-lab evidence all pass.
