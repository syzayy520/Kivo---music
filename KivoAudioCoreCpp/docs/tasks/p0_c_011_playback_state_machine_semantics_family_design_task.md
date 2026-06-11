# P0-C-011 Playback State Machine Semantics — Family Design Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-011-PLAYBACK-STATE-MACHINE-SEMANTICS-FAMILY-DESIGN-TASKBOOK-001`  
**Type:** planning-taskbook / family-design-input / no-runtime / no-implementation  
**Repository:** `syzayy520/Kivo---music`  
**Project root:** `KivoAudioCoreCpp/`  
**Target phase:** P0-C Core Contract Foundation  
**Previous sealed code task:** P0-C-010 Error Taxonomy / Recovery Matrix  
**P0-C-010 sealed code baseline:** `0ef9fd2515caad9ed51d57f0e9ac10bd93ac2184`  
**This document:** docs-only handoff for the next P0-C-011 planning pass  
**Next expected task:** P0-C-011 family-design draft, not implementation

---

## 0. Purpose

This taskbook is written into the repository so the next assistant can see exactly what P0-C-011 is supposed to do before touching code.

P0-C-011 is **Playback State Machine Semantics**. It must define the core semantic language for lifecycle states, transition legality, transition rejection, and documented edge-case state behavior.

This taskbook is **not** an implementation ticket. The next assistant must first produce a planning draft and wait for Owner approval before creating or modifying production/test files.

Required approval phrase before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

No implementation may begin without that exact Owner approval.

---

## 1. Source of Truth

The current P0-C task plan identifies P0-C-011 as:

```text
P0-C-011: Playback State Machine Semantics
Location: src/core/contract/state/
```

The current C++ repository convention must be used instead of the older `src/core/contract/` wording:

```text
include/kivo/core/contract/state/
tests/contracts/state/
```

Required states from the task plan and enhancement input:

```text
Idle, Opening, Ready, Playing, Pausing, Paused,
Seeking, Draining, Recovering, Stopped, Failed, Closed
```

Required edge/illegal transition semantics to define:

```text
Playing -> Opening          forbidden
Seeking -> Seeking          merge or queue must be defined
Recovering -> Seek          reject or queue must be defined
Draining -> Pause           behavior must be defined
Failed -> Close             must succeed
Closed -> any mutation      must fail
Device lost during seek     must define
Host shutdown during drain  must define
```

---

## 2. Baseline Gate for the Next Assistant

This taskbook itself is a docs-only commit after the P0-C-010 code commit. Therefore the next assistant must **not** require local HEAD to equal the P0-C-010 code commit exactly. Instead, it must verify that the current master contains the sealed P0-C-010 baseline and is synced.

Run:

```powershell
git remote -v
git status --short --branch --untracked-files=all
git branch --show-current
git rev-parse HEAD
git ls-remote origin master
git merge-base --is-ancestor 0ef9fd2515caad9ed51d57f0e9ac10bd93ac2184 HEAD; if ($LASTEXITCODE -ne 0) { exit 1 }
```

Required baseline:

```text
origin == https://github.com/syzayy520/Kivo---music.git
branch == master
working tree clean
local HEAD == remote master HEAD
0ef9fd2515caad9ed51d57f0e9ac10bd93ac2184 is an ancestor of HEAD
```

If any check fails, STOP.

---

## 3. Mandatory Workflow

The next assistant must follow this order:

```text
1. Base Gate
2. Inventory Gate
3. ODR / Existing Family Gate
4. Natural Family Tree Draft
5. Owner review
6. Owner approval using PLANNING_APPROVED_FOR_IMPLEMENTATION
7. Implementation baseline
8. Build + CTest + gates
9. commit/push
10. final verification
```

This taskbook only authorizes steps 1 through 4.

The next assistant must not create source/test files during the planning-only pass.

---

## 4. P0-C-011 Scope

P0-C-011 should define contract-only types for:

```text
state lifecycle phase
state stability / terminality
transition description
transition validity
transition decision
transition rejection reason
edge-case transition rules
state semantics contract summary
```

P0-C-011 should answer:

```text
What states exist?
Which states are stable, transitional, failed, or terminal?
What is a transition from one state to another?
Is a transition allowed, forbidden, queued, merged, rejected, deferred, or conditional?
Which edge-case rules are explicitly documented?
How are illegal transitions represented as contract values?
```

P0-C-011 must not execute transitions. It must not become a runtime state machine engine.

---

## 5. Explicit Non-goals

P0-C-011 must not implement:

```text
no runtime state machine engine
no mutable current-state holder
no command queue
no cancellation engine
no retry loop
no thread
no sleep / wait
no I/O
no source open/close
no audio device open/close
no render queue mutation
no decode queue mutation
no callback dispatch
no host event dispatch
no logging sink
no telemetry sink
no error recovery execution
no platform mapping
no Windows / WASAPI / COM / FFmpeg / mpv API
no UI behavior
no media library behavior
```

P0-C-011 is only the state semantics contract layer.

---

## 6. Boundary With Neighboring P0-C Tickets

### 6.1 Boundary with P0-C-010 Error Taxonomy / Recovery Matrix

P0-C-010 defines error taxonomy and recovery matrix contracts.

P0-C-011 may define values such as `Failed` and `Recovering`, but it must not define error domains, recovery actions, or recovery matrix logic.

Default rule: no direct include from `include/kivo/core/contract/error/**`.

### 6.2 Boundary with P0-C-012 Command Ordering / Cancellation / User Race

P0-C-012 will define command ordering and cancellation semantics.

P0-C-011 may define transition intent or transition cause at a semantic level, but it must not define command queues, command cancellation, generation ownership, or UI race behavior.

If a type starts to look like command execution, move it out of P0-C-011.

### 6.3 Boundary with P0-C-013 Observability / Telemetry / Ring Trace

P0-C-011 must not define metrics, ring trace payloads, or telemetry event IDs.

### 6.4 Boundary with Future Runtime Core

P0-C-011 must not define a class that stores current state or applies transitions. It must not implement functions like `apply`, `step`, `dispatch`, `advance`, `handle`, `execute`, or `process`.

---

## 7. Inventory Gate Required for Next Assistant

The next assistant must inspect current repository state before drafting final planning:

```powershell
Test-Path docs/tasks/p0_c_core_contract_foundation_task.md
Test-Path docs/architecture/p0c_through_p0i_enhancement_input.md

Get-ChildItem include/kivo/core/contract -Recurse
Get-ChildItem tests/contracts -Recurse

Get-Content docs/tasks/p0_c_core_contract_foundation_task.md
Get-Content docs/architecture/p0c_through_p0i_enhancement_input.md
Get-Content CMakeLists.txt
Get-Content tests/contracts/contract_tests_main.cpp
Get-Content tools/gates/check_contract_genealogy_gate.ps1
Get-Content tools/gates/check_forbidden_token_gate.ps1
Get-Content tools/gates/check_no_audio_runtime_gate.ps1
Get-Content tools/gates/check_audio_core_scope_lock_gate.ps1
```

The planning draft must report:

```text
1. Existing contract families
2. Whether include/kivo/core/contract/state/ already exists
3. Whether tests/contracts/state/ already exists
4. Existing state/transition/phase/command/recovery names
5. Current CMake test pattern
6. Current contract_tests_main runner pattern
7. Current genealogy gate pattern for new P0-C families
```

---

## 8. ODR / Existing Name Gate

The next assistant must run:

```powershell
rg -n "CoreState|PlaybackState|StateMachine|StateTransition|Transition|TransitionRule|IllegalTransition|StateRejection|RejectionReason|TerminalState|StableState|TransientState|Opening|Playing|Pausing|Paused|Seeking|Draining|Recovering|Stopped|Failed|Closed|CloseRule|DrainPause|SeekDuringRecovery|DeviceLostDuringSeek|ShutdownDuringDrain" include/kivo/core/contract tests/contracts
```

The planning draft must report:

```text
1. Same-name types
2. Similar semantic types
3. ODR risks
4. Names to avoid
5. Whether state/ already exists
6. Whether command/ already exists
7. Whether any P0-C-012 command concepts are being pulled too early
```

If same-name production types already exist, STOP.

---

## 9. Recommended Natural Family Tree Candidate

The following is a proposed design candidate. The next assistant must verify and may refine it during planning. Do not implement blindly.

Recommended top-level family:

```text
include/kivo/core/contract/state/
tests/contracts/state/
```

Recommended production tree:

```text
include/kivo/core/contract/state/
  phase/
    core_state.hpp
    state_stability.hpp
    state_terminality.hpp
  transition/
    state_transition_intent.hpp
    state_transition.hpp
    state_transition_validity.hpp
    state_transition_decision.hpp
  rejection/
    state_transition_rejection_reason.hpp
    illegal_transition_rule.hpp
  rule/
    reentrant_transition_policy.hpp
    transition_preemption_policy.hpp
    terminal_state_rule.hpp
  scenario/
    seeking_reentry_rule.hpp
    recovering_seek_rule.hpp
    draining_pause_rule.hpp
    failed_close_rule.hpp
    closed_mutation_rule.hpp
    device_lost_during_seek_rule.hpp
    shutdown_during_drain_rule.hpp
  contract/
    state_machine_contract.hpp
```

Recommended test tree:

```text
tests/contracts/state/
  phase/
    state_phase_tests.cpp
  transition/
    state_transition_tests.cpp
  rejection/
    state_rejection_tests.cpp
  rule/
    state_rule_tests.cpp
  scenario/
    state_scenario_tests.cpp
  contract/
    state_machine_contract_tests.cpp
```

Recommended file count candidate:

```text
Production headers: 20
Test files: 6
Modified files: 3
  - CMakeLists.txt
  - tests/contracts/contract_tests_main.cpp
  - tools/gates/check_contract_genealogy_gate.ps1
Total touched: 29
```

The next assistant must validate whether this count is appropriate. It may reduce count if some files are too thin or split further if a file has mixed responsibility.

---

## 10. Candidate Type Plan

### phase/core_state.hpp

Purpose: one enum for required lifecycle states.

Candidate:

```cpp
enum class CoreState : uint8_t {
    Unknown = 0,
    Idle,
    Opening,
    Ready,
    Playing,
    Pausing,
    Paused,
    Seeking,
    Draining,
    Recovering,
    Stopped,
    Failed,
    Closed
};
```

Do not name the enum `PlaybackState` unless planning explicitly approves it. `CoreState` is preferred because it avoids coupling the type name to UI or product wording.

### phase/state_stability.hpp

Purpose: classify whether a state is stable or transitional.

Candidate:

```cpp
enum class StateStability : uint8_t {
    Unknown = 0,
    Stable,
    Transitional,
    Terminal,
    Failed
};
```

### phase/state_terminality.hpp

Purpose: classify how a state behaves at end-of-life.

Candidate:

```cpp
enum class StateTerminality : uint8_t {
    Unknown = 0,
    NotTerminal,
    StoppedTerminal,
    FailedTerminal,
    ClosedTerminal
};
```

### transition/state_transition_intent.hpp

Purpose: semantic cause of a transition without implementing command ordering.

Candidate:

```cpp
enum class StateTransitionIntent : uint8_t {
    Unknown = 0,
    Open,
    Prepare,
    Start,
    Pause,
    Resume,
    Seek,
    Drain,
    Recover,
    Stop,
    Close,
    Fail,
    Reset
};
```

Do not implement command queues or cancellation in this file.

### transition/state_transition.hpp

Purpose: value object describing a transition.

Candidate fields:

```text
CoreState from
CoreState to
StateTransitionIntent intent
```

Allowed methods:

```text
operator== only
```

No `is_*`, no `apply`, no `execute`, no state mutation.

### transition/state_transition_validity.hpp

Purpose: classify transition legality.

Candidate:

```cpp
enum class StateTransitionValidity : uint8_t {
    Unknown = 0,
    Allowed,
    Forbidden,
    Conditional,
    MergeRequired,
    QueueRequired,
    RejectRequired,
    CompleteCurrentFirst
};
```

### transition/state_transition_decision.hpp

Purpose: final semantic decision for a requested transition.

Candidate:

```cpp
enum class StateTransitionDecision : uint8_t {
    Unknown = 0,
    Allow,
    Reject,
    Queue,
    Merge,
    Defer,
    ForceClose
};
```

This is a contract value only, not an executor.

### rejection/state_transition_rejection_reason.hpp

Purpose: explain why a transition is rejected.

Candidate:

```cpp
enum class StateTransitionRejectionReason : uint8_t {
    Unknown = 0,
    InvalidSourceState,
    InvalidTargetState,
    TerminalState,
    AlreadyInRequestedState,
    ConflictingTransition,
    RequiresDrainCompletion,
    RequiresRecoveryCompletion,
    ClosedStateRejectsMutation,
    FailedStateAllowsCloseOnly
};
```

### rejection/illegal_transition_rule.hpp

Purpose: define illegal transition rule classification.

Candidate fields:

```text
StateTransition transition
StateTransitionValidity validity
StateTransitionRejectionReason rejection_reason
```

No logic.

### rule/reentrant_transition_policy.hpp

Purpose: define behavior for repeated transition requests such as Seeking -> Seeking.

Candidate:

```cpp
enum class ReentrantTransitionPolicy : uint8_t {
    Unknown = 0,
    Forbid,
    Merge,
    Queue,
    ReplacePending,
    KeepLatest
};
```

### rule/transition_preemption_policy.hpp

Purpose: define whether one transition can interrupt another.

Candidate:

```cpp
enum class TransitionPreemptionPolicy : uint8_t {
    Unknown = 0,
    NeverPreempt,
    AllowSafePreempt,
    QueueUntilCurrentCompletes,
    RejectUntilCurrentCompletes,
    ForceCloseOnly
};
```

### rule/terminal_state_rule.hpp

Purpose: define behavior for terminal states.

Candidate:

```cpp
enum class TerminalStateRule : uint8_t {
    Unknown = 0,
    RejectAllMutations,
    AllowCloseOnly,
    AllowInspectOnly,
    AlreadyClosedIsIdempotent
};
```

### scenario/seeking_reentry_rule.hpp

Purpose: required rule for Seeking -> Seeking.

Candidate:

```cpp
enum class SeekingReentryRule : uint8_t {
    Unknown = 0,
    MergeSeek,
    QueueSeek,
    ReplaceWithLatestSeek,
    RejectNewSeek
};
```

### scenario/recovering_seek_rule.hpp

Purpose: required rule for Recovering -> Seek.

Candidate:

```cpp
enum class RecoveringSeekRule : uint8_t {
    Unknown = 0,
    RejectSeek,
    QueueSeek,
    DeferUntilRecovered,
    FailIfRecoveryRequired
};
```

### scenario/draining_pause_rule.hpp

Purpose: required rule for Draining -> Pause.

Candidate:

```cpp
enum class DrainingPauseRule : uint8_t {
    Unknown = 0,
    RejectPause,
    QueuePause,
    CompleteDrainFirst,
    ConvertToStop
};
```

### scenario/failed_close_rule.hpp

Purpose: required rule for Failed -> Close.

Candidate:

```cpp
enum class FailedCloseRule : uint8_t {
    Unknown = 0,
    CloseMustSucceed,
    CloseIsIdempotent,
    CleanupThenClose
};
```

### scenario/closed_mutation_rule.hpp

Purpose: required rule for Closed -> any mutation.

Candidate:

```cpp
enum class ClosedMutationRule : uint8_t {
    Unknown = 0,
    RejectMutation,
    ReturnAlreadyClosed,
    InspectOnly
};
```

Do not call this `ClosedWriteRule` unless planning approves. `Mutation` is preferred because it avoids tying this contract to a writer/runtime path.

### scenario/device_lost_during_seek_rule.hpp

Purpose: required rule for device-loss semantics during seek.

Candidate:

```cpp
enum class DeviceLostDuringSeekRule : uint8_t {
    Unknown = 0,
    EnterRecovering,
    FailSeek,
    PreserveSeekTarget,
    RequireRecoveryThenSeek
};
```

This is a state semantics rule only. It does not implement device recovery.

### scenario/shutdown_during_drain_rule.hpp

Purpose: required rule for shutdown during drain.

Candidate:

```cpp
enum class ShutdownDuringDrainRule : uint8_t {
    Unknown = 0,
    CompleteDrainThenClose,
    AbortDrainThenClose,
    ForceClose,
    MarkClosedIdempotent
};
```

### contract/state_machine_contract.hpp

Purpose: aggregate state semantics summary.

Candidate fields:

```text
SeekingReentryRule seeking_reentry
RecoveringSeekRule recovering_seek
DrainingPauseRule draining_pause
FailedCloseRule failed_close
ClosedMutationRule closed_mutation
DeviceLostDuringSeekRule device_lost_during_seek
ShutdownDuringDrainRule shutdown_during_drain
```

Allowed methods:

```text
operator== only
```

No state transition evaluator.

---

## 11. Token Hygiene and Source Comment Rules

Future `.hpp` and `.cpp` files must not copy planning explanation into comments.

Allowed comments:

```text
Contract: core state values
Contract: state transition value
Contract: terminal state rule
```

Avoid comments like:

```text
No runtime state machine engine
No device recovery
No command queue
```

Those planning explanations belong in this taskbook, not in source/test files.

Preferred source terms:

```text
CoreState
StateTransition
StateTransitionIntent
StateTransitionDecision
StateTransitionRejectionReason
ClosedMutationRule
```

Avoid unless planning explicitly approves:

```text
PlaybackState
RuntimeStateMachine
CommandQueue
WriterState
RendererState
DecoderState
```

---

## 12. Forbidden Direct Include Policy

Default production headers may include:

```text
<cstdint>
```

Composite structs may include sibling state-family headers only.

Default forbidden direct includes:

```text
include/kivo/core/contract/error/**
include/kivo/core/contract/queue/**
include/kivo/core/contract/buffer/**
include/kivo/core/contract/output/**
include/kivo/core/contract/clock/**
include/kivo/core/contract/seek/**
include/kivo/core/contract/format/**
include/kivo/core/contract/source/**
include/kivo/core/contract/cue/**
include/kivo/core/contract/realtime/**
include/kivo/core/contract/capability/**
include/kivo/core/contract/generation/**
```

Also forbidden:

```text
src/**
include/kivo/core/audio/**
include/kivo/core/device/**
include/kivo/core/output/**
include/kivo/core/source/**
include/kivo/core/reader/**
include/kivo/core/container/**
include/kivo/core/demuxer/**
include/kivo/core/decoder/**
include/kivo/core/pipeline/**
include/kivo/core/render/**
include/kivo/core/platform/**
include/kivo/core/runtime/**
```

If the planning draft claims a direct include is required, it must justify the include and wait for Owner review.

---

## 13. Forbidden Runtime / Platform Tokens for Future Source and Tests

The future implementation must define a scan for `state/` source and test files.

Forbidden in future `.hpp` and `.cpp` content:

```text
WASAPI
Windows
WAVEFORMATEX
WAVEFORMATEXTENSIBLE
FFmpeg
AVFrame
AVPacket
mpv
COM
IAudioClient
IAudioRenderClient
RenderFrame
RenderTarget
surface
swapchain
framebuffer
video renderer
MMCSS
AVRT
HRESULT
Win32
```

Forbidden implementation-oriented terms in future state files unless Owner explicitly approves a token-safe exception:

```text
runtime engine
state engine
command queue
retry loop
backoff timer
thread
sleep
wait
open file
read file
fopen
ifstream
socket
network
parser
demuxer
decoder
render writer
state executor
transition executor
callback dispatch
logger
telemetry sink
```

The next assistant must avoid false positives by not copying this list into source/test comments.

---

## 14. Suggested Test Plan

If the candidate tree is accepted, use six test files:

```text
tests/contracts/state/phase/state_phase_tests.cpp
tests/contracts/state/transition/state_transition_tests.cpp
tests/contracts/state/rejection/state_rejection_tests.cpp
tests/contracts/state/rule/state_rule_tests.cpp
tests/contracts/state/scenario/state_scenario_tests.cpp
tests/contracts/state/contract/state_machine_contract_tests.cpp
```

Suggested coverage:

```text
phase tests:
  CoreState required values
  StateStability values
  StateTerminality values

transition tests:
  StateTransitionIntent values
  StateTransition default construction
  StateTransition field modification
  StateTransition equality
  StateTransitionValidity values
  StateTransitionDecision values

rejection tests:
  StateTransitionRejectionReason values
  IllegalTransitionRule default construction
  IllegalTransitionRule field modification
  IllegalTransitionRule equality

rule tests:
  ReentrantTransitionPolicy values
  TransitionPreemptionPolicy values
  TerminalStateRule values

scenario tests:
  SeekingReentryRule values
  RecoveringSeekRule values
  DrainingPauseRule values
  FailedCloseRule values
  ClosedMutationRule values
  DeviceLostDuringSeekRule values
  ShutdownDuringDrainRule values

contract tests:
  StateMachineContract default construction
  StateMachineContract field modification
  StateMachineContract equality
  Required edge-case fields exist
```

All tests must be value-semantic contract tests only.

---

## 15. Gate Update Plan Candidate

If the candidate tree is accepted, implementation will modify:

```text
KivoAudioCoreCpp/tools/gates/check_contract_genealogy_gate.ps1
```

Expected additive changes:

```text
1. Add "state" to allowed production dirs.
2. Add "state" to allowed test dirs.
3. Add state production subdir allowlist.
4. Add state test subdir allowlist.
```

Candidate production allowlist:

```powershell
$stateSubdirAllowlists = @{
    "phase" = @(
        "core_state.hpp",
        "state_stability.hpp",
        "state_terminality.hpp"
    )
    "transition" = @(
        "state_transition_intent.hpp",
        "state_transition.hpp",
        "state_transition_validity.hpp",
        "state_transition_decision.hpp"
    )
    "rejection" = @(
        "state_transition_rejection_reason.hpp",
        "illegal_transition_rule.hpp"
    )
    "rule" = @(
        "reentrant_transition_policy.hpp",
        "transition_preemption_policy.hpp",
        "terminal_state_rule.hpp"
    )
    "scenario" = @(
        "seeking_reentry_rule.hpp",
        "recovering_seek_rule.hpp",
        "draining_pause_rule.hpp",
        "failed_close_rule.hpp",
        "closed_mutation_rule.hpp",
        "device_lost_during_seek_rule.hpp",
        "shutdown_during_drain_rule.hpp"
    )
    "contract" = @(
        "state_machine_contract.hpp"
    )
}
```

Candidate test allowlist:

```powershell
$stateTestSubdirAllowlists = @{
    "phase" = @("state_phase_tests.cpp")
    "transition" = @("state_transition_tests.cpp")
    "rejection" = @("state_rejection_tests.cpp")
    "rule" = @("state_rule_tests.cpp")
    "scenario" = @("state_scenario_tests.cpp")
    "contract" = @("state_machine_contract_tests.cpp")
}
```

The next assistant must verify this against the current genealogy gate style before implementation.

---

## 16. Required Planning Draft Output

The next assistant must output a planning draft containing:

```text
1. Base Gate raw output
2. Inventory Gate conclusion
3. ODR / Existing Family Gate conclusion
4. P0-C-011 true goal and non-goals
5. Natural Family Tree Plan
6. Folder -> Subfamily -> Single-file Responsibility Table
7. Production Header Inventory
8. Test File Inventory
9. Type -> File Matrix
10. Candidate Field Plan
11. Direct Include Allowlist
12. Forbidden Include List
13. Forbidden Token / Runtime Scan Plan
14. CMakeLists.txt Impact
15. contract_tests_main.cpp Impact
16. Contract Genealogy Gate Update Plan
17. Boundary with P0-C-010 Error Taxonomy
18. Boundary with P0-C-012 Command Ordering
19. Risk Assessment
20. STOP Conditions
21. Implementation Readiness Verdict
```

The draft must end with:

```text
KIVO-AUDIO-CORE-P0-C-011-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```

It must not claim implementation success.

---

## 17. STOP Conditions

Any of these must stop the next assistant:

```text
STOP_BASELINE_NOT_SYNCED_WITH_REMOTE
STOP_P0_C_010_BASELINE_NOT_ANCESTOR
STOP_WORKSPACE_NOT_CLEAN
STOP_EXISTING_STATE_FAMILY_FOUND
STOP_EXISTING_COMMAND_FAMILY_FOUND_WHEN_PLANNING_STATE
STOP_ODR_NAME_COLLISION
STOP_SCOPE_UNCLEAR
STOP_RUNTIME_STATE_MACHINE_ENGINE_LEAK
STOP_COMMAND_QUEUE_LEAK
STOP_CANCELLATION_ENGINE_LEAK
STOP_RETRY_LOOP_LEAK
STOP_DEVICE_RECOVERY_LEAK
STOP_RENDER_QUEUE_MUTATION_LEAK
STOP_SOURCE_OPEN_CLOSE_LEAK
STOP_PLATFORM_API_LEAK
STOP_FORBIDDEN_DIRECT_INCLUDE_REQUIRED
STOP_IMPLEMENTATION_ATTEMPTED_DURING_PLANNING
```

---

## 18. Implementation Guardrails After Approval

After Owner approval, implementation must remain contract-only.

Expected modified files:

```text
KivoAudioCoreCpp/CMakeLists.txt
KivoAudioCoreCpp/tests/contracts/contract_tests_main.cpp
KivoAudioCoreCpp/tools/gates/check_contract_genealogy_gate.ps1
```

Expected created roots:

```text
KivoAudioCoreCpp/include/kivo/core/contract/state/**
KivoAudioCoreCpp/tests/contracts/state/**
```

No other files should be touched without an explicit Owner-approved revised allowlist.

---

## 19. Verification Requirements After Future Implementation

Future implementation must provide:

```text
CMake configure PASS
CMake build PASS
CTest PASS
contract tests PASS
capability tests PASS
aggregate gates 9/9 PASS
forbidden token scan PASS
standalone state-family runtime scan PASS
git diff --check PASS
git show --stat --oneline --name-status -1 proof
git rev-parse HEAD proof
git ls-remote origin master proof
git status --short --branch --untracked-files=all proof
```

Standalone state-family scan should check only future `state/` source and test content, not this docs file.

---

## 20. Authoring Assessment

P0-C-011 is conceptually important and should not be under-designed. The state contract must define edge-case semantics before runtime playback code exists.

The safest design is a value-only state semantics vocabulary:

```text
states + transition values + legality + scenario rules + aggregate contract
```

The risk to avoid is creating a hidden runtime state machine engine. P0-C-011 should provide language, not behavior.

Recommended next action:

```text
Next assistant should read this taskbook, run Inventory/ODR gates, then produce P0-C-011 Planning Draft v1.0 for Owner review.
```

Do not proceed to implementation until the Owner explicitly approves the planning draft.
