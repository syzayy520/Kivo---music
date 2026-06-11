# P0-D-014 BufferLifetimeProof Collision Repair

**Task ID:** `KIVO-AUDIO-CORE-P0-D-014-BUFFER-LIFETIME-PROOF-COLLISION-REPAIR-TASK-001`

## Status

```text
REPAIR
CONTRACT_HEADER_REPAIR
NO_RUNTIME_CHANGE
```

## Goal

Repair the same-namespace public type collision between the general buffer lifetime proof contract and the realtime buffer lifetime requirement contract.

## Natural family tree

```text
KivoAudioCoreCpp/
  include/
    kivo/core/contract/
      buffer/lifetime/buffer_lifetime_proof.hpp
      realtime/buffer/buffer_lifetime_rule.hpp
  tests/
    contracts/
      realtime/buffer/realtime_buffer_contract_tests.cpp
      composability/public_header_composability_tests.cpp
```

## Scope

Allowed files:

```text
KivoAudioCoreCpp/include/kivo/core/contract/realtime/buffer/buffer_lifetime_rule.hpp
KivoAudioCoreCpp/tests/contracts/realtime/buffer/realtime_buffer_contract_tests.cpp
KivoAudioCoreCpp/tests/contracts/composability/public_header_composability_tests.cpp
KivoAudioCoreCpp/docs/tasks/p0_d_014_buffer_lifetime_proof_collision_repair_task.md
```

## Acceptance criteria

```text
- General BufferLifetimeProof remains unchanged.
- Realtime contract uses a distinct type name.
- Realtime tests use the distinct realtime type.
- Public header composability test can include both headers.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-014-BUFFER-LIFETIME-PROOF-COLLISION-REPAIR
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
