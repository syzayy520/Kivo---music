# Audio Core Scope Lock Policy

**Purpose:** Lock the KivoAudioCoreCpp repository to audio-only scope. Prevent video renderer, video surface, video HDR pipeline, and out-of-scope device contract drift.  
**Scope:** All phases from P0-012 onward.  
**Current Phase Rule:** P0-012 completed. No P0-013/P0-014 implementation allowed until explicit allowlist approval.  
**Last Updated:** 2026-06-11 (KIVO-AUDIO-CORE-SCOPE-LOCK-001)

## 1. Purpose

This policy enforces that KivoAudioCoreCpp is exclusively a **music player audio core**. It prevents scope drift into video rendering, video surfaces, video HDR pipelines, and out-of-scope device contracts. This policy was created after a scope drift audit detected no active drift but identified the need for preventive governance.

## 2. Scope

Applies to all source code, tests, documentation, gates, and CMake files within KivoAudioCoreCpp/.

## 3. Current Phase Rule (Post-P0-012)

P0-012 (Capability Negotiation Foundation) is the last completed task. No further implementation is authorized until:
- Audio core scope lock is established (this policy)
- Aggregate gates include scope lock gate
- Base Gate passes at new task start

## 4. Allowed (Audio Core Scope)

The following domains are within the authorized audio core scope:

### 4.1 Audio Capability / Negotiation
- `include/kivo/core/contract/capability/` (25 headers)
- `tests/contracts/capability/` (7 test files)
- `kivo_capability_tests` CMake target

### 4.2 Audio Contract Foundation
- `include/kivo/core/contract/` (result, generation_id, sample_position, format, clock, seek)
- `tests/contracts/` (foundation, format, clock, seek)
- `kivo_contract_tests` CMake target

### 4.3 Audio Source / IO (future phases)
- Audio file reading
- Audio stream input
- Audio buffer management

### 4.4 Audio Clock / Timeline (future phases)
- Audio clock domain
- Audio timeline tracking
- Audio position management

### 4.5 Audio Buffer / Memory (future phases)
- Audio buffer capability
- Audio memory management
- Audio pool budgets

### 4.6 Audio Seek / Flush / Drain (future phases)
- Audio seek operations
- Audio flush scope
- Audio drain semantics

### 4.7 Audio Output Route (future phases)
- Audio device output
- Audio output capability
- Audio output mode

### 4.8 Audio Device / Output Contract (future phases)
- Audio device capability (NOT video device)
- Audio output contract (NOT video output)
- Audio device memory (NOT video memory)

### 4.9 WASAPI Adapter Shell (future phases)
- WASAPI audio output adapter (audio-only)
- WASAPI audio render client (audio-only, NOT video render)

### 4.10 Decode Seam (future phases)
- Audio decode contract
- FFmpeg audio decode seam (audio-only)

### 4.11 Recovery / Telemetry (future phases)
- Audio playback recovery
- Audio telemetry contracts
- Audio health monitoring

## 5. Forbidden (Video / Out-of-Scope)

The following are **strictly forbidden** in this repository:

### 5.1 Video Renderer
- `Video Renderer` token
- `RenderFrame` token
- `RenderTarget` token
- `RenderSubmission` token
- `RenderOutcome` token

### 5.2 Video Surface / Swapchain
- `video surface` token
- `swapchain` token
- `framebuffer` token

### 5.3 Video HDR Pipeline
- `video HDR` token
- `HDR pipeline` token
- `HDR metadata` token (video context)

### 5.4 Video Render Contracts
- `src/render/contracts` path
- `tests/render/contracts` path
- Any directory named `render/` containing video-related contracts

### 5.5 P0-014 Video Renderer Contracts
- P0-014 is **OUT_OF_SCOPE_VIDEO_RENDERER_DISCARDED**
- No P0-014 planning, implementation, or documentation
- No video renderer contract types
- No video renderer test files

### 5.6 P0-013 Device Contracts (Conditional Block)
- P0-013 is **NO_GO_ALLOWLIST_MISSING_NO_IMPLEMENTATION**
- No `src/device/contracts` directory creation
- No `tests/device/contracts` directory creation
- No device contract implementation unless:
  1. Complete allowlist provided
  2. Dependency proof provided
  3. Audio-only confirmation provided
  4. Explicit project owner approval

### 5.7 General Forbidden Patterns
- Any video rendering code
- Any video surface management
- Any video HDR processing
- Any video device enumeration
- Any video output routing

## 6. Gate / Check Method

### 6.1 Scope Lock Gate
- `check_audio_core_scope_lock_gate.ps1` scans for forbidden video/out-of-scope tokens
- Gate must PASS before any new task implementation
- Gate failure is a **HARD STOP**

### 6.2 Integration with Aggregate Gates
- Scope lock gate is added to `run_cpp_audio_core_gates.ps1`
- All 9 gates must PASS for overall PASS

### 6.3 Allowed Tokens (No False Positives)
The gate must NOT flag these legitimate audio core tokens:
- `kivo_capability_tests`
- `kivo_contract_tests`
- `include/kivo/core/contract/capability`
- `audio` (in any context)
- `clock` (audio clock domain)
- `seek` (audio seek operations)
- `format` (audio format descriptor)
- `source` (audio source)
- `memory` (audio memory capability)
- `output` (audio output capability)
- `render` in `RenderClientBoundary` context (audio render client, NOT video render)

## 7. Failure Classification

- `VIDEO_RENDERER_SCOPE_DRIFT_BLOCKER`: Video renderer token detected
- `VIDEO_SURFACE_SCOPE_DRIFT_BLOCKER`: Video surface/swapchain token detected
- `VIDEO_HDR_SCOPE_DRIFT_BLOCKER`: Video HDR pipeline token detected
- `VIDEO_RENDER_CONTRACTS_BLOCKER`: Video render contract path detected
- `P0_014_SCOPE_DRIFT_BLOCKER`: P0-014 video renderer content detected
- `P0_013_UNAUTHORIZED_BLOCKER`: P0-013 device contract without allowlist

## 8. P0-013 Conditional Unblocking

P0-013 (Device Contracts) may be unblocked ONLY if:

1. **Allowlist Provided**: Complete list of device contract types with justification
2. **Dependency Proof**: Proof that device contracts depend only on audio core contracts
3. **Audio-Only Confirmation**: Confirmation that device contracts are audio device only (NOT video device)
4. **Owner Approval**: Explicit project owner sign-off

If all 4 conditions are met, P0-013 status changes from `NO_GO_ALLOWLIST_MISSING_NO_IMPLEMENTATION` to `GO_WITH_ALLOWLIST`.

## 9. P0-014 Permanent Disposition

P0-014 (Video Renderer Contracts) is **permanently discarded**:
- Status: `OUT_OF_SCOPE_VIDEO_RENDERER_DISCARDED`
- No future planning
- No future implementation
- No future revival
- Video rendering is not within Kivo music player audio core scope

## 10. Scope Lock Enforcement

### 10.1 Pre-Task Base Gate
Before starting any new implementation task:
1. Run `git status` — confirm clean
2. Run `git rev-parse HEAD` — confirm at known safe point
3. Run scope lock gate — confirm PASS
4. Run aggregate gates — confirm all PASS

### 10.2 During Implementation
- No video tokens may be introduced
- No video paths may be created
- No P0-013/P0-014 content may be added
- Any scope violation is a HARD STOP

### 10.3 Post-Implementation Verification
- Run scope lock gate again
- Run aggregate gates again
- Confirm no forbidden tokens introduced
- Confirm no forbidden paths created

## 11. Audit Trail

| Date | Event | Classification |
|------|-------|----------------|
| 2026-06-11 | Scope drift audit completed | AUDIT_ONLY_COMPLETE_NO_REPO_DAMAGE_DRAFT_ONLY_DISCARDED |
| 2026-06-11 | P0-014 permanently discarded | OUT_OF_SCOPE_VIDEO_RENDERER_DISCARDED |
| 2026-06-11 | P0-013 conditionally blocked | NO_GO_ALLOWLIST_MISSING_NO_IMPLEMENTATION |
| 2026-06-11 | Scope lock policy created | KIVO-AUDIO-CORE-SCOPE-LOCK-001 |
| 2026-06-11 | Scope lock gate created | KIVO-AUDIO-CORE-SCOPE-LOCK-001 |

## 12. Deferred Items

- Formal scope drift CI integration
- Automated video token detection in pre-commit hooks
- Scope violation reporting dashboard

---

**Iron Rule 2:** This repository is exclusively for Kivo music player audio core. Video rendering, video surfaces, video HDR pipelines, and out-of-scope device contracts are permanently forbidden.
