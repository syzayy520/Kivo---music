# P0-D-016 Gate Build Directory Exclusion Repair

**Task ID:** `KIVO-AUDIO-CORE-P0-D-016-GATE-BUILD-DIRECTORY-EXCLUSION-REPAIR-TASK-001`

## Status

```text
REPAIR
GOVERNANCE_GATE_REPAIR
NO_RUNTIME_CHANGE
```

## Goal

Repair governance gates so generated build directories and validation documentation/tooling families are classified correctly.

## Natural family tree

```text
KivoAudioCoreCpp/
  tools/
    gates/
      check_genealogy_gate.ps1
      check_no_empty_forest_gate.ps1
      check_no_audio_runtime_gate.ps1
  docs/
    tasks/
      p0_d_016_gate_build_directory_exclusion_repair_task.md
```

## Scope

Allowed files:

```text
KivoAudioCoreCpp/tools/gates/check_genealogy_gate.ps1
KivoAudioCoreCpp/tools/gates/check_no_empty_forest_gate.ps1
KivoAudioCoreCpp/tools/gates/check_no_audio_runtime_gate.ps1
KivoAudioCoreCpp/docs/tasks/p0_d_016_gate_build_directory_exclusion_repair_task.md
```

## Acceptance criteria

```text
- build/ is allowed as generated local output at top level.
- docs/validation/ is allowed as validation documentation family.
- tools/validation/ is allowed as validation tooling family.
- No-empty-forest gate skips build/ recursively.
- No-runtime gate skips build/ recursively.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-016-GATE-BUILD-DIRECTORY-EXCLUSION-REPAIR
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
