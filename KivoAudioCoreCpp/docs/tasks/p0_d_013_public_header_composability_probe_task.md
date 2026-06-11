# P0-D-013 Public Header Composability Probe

**Task ID:** `KIVO-AUDIO-CORE-P0-D-013-PUBLIC-HEADER-COMPOSABILITY-PROBE-TASK-001`

## Status

```text
IMPLEMENTATION
CONTRACT_TEST_ONLY
NO_RUNTIME_CHANGE
```

## Goal

Add a focused contract test translation unit that includes public contract headers together to catch same-namespace type collisions that isolated tests can miss.

## Natural family tree

```text
KivoAudioCoreCpp/
  tests/
    contracts/
      composability/
        public_header_composability_tests.cpp
  docs/
    tasks/
      p0_d_013_public_header_composability_probe_task.md
```

## Scope

Allowed files:

```text
KivoAudioCoreCpp/tests/contracts/composability/public_header_composability_tests.cpp
KivoAudioCoreCpp/tests/contracts/contract_tests_main.cpp
KivoAudioCoreCpp/CMakeLists.txt
KivoAudioCoreCpp/docs/tasks/p0_d_013_public_header_composability_probe_task.md
```

## Acceptance criteria

```text
- New test translation unit includes buffer lifetime and realtime buffer lifetime headers together.
- Test is registered in kivo_contract_tests.
- No runtime code is introduced.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-013-PUBLIC-HEADER-COMPOSABILITY-PROBE
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
