# P0-D-015 PCM Format Descriptor Container Consistency

**Task ID:** `KIVO-AUDIO-CORE-P0-D-015-PCM-FORMAT-DESCRIPTOR-CONTAINER-CONSISTENCY-TASK-001`

## Status

```text
REPAIR
CONTRACT_HEADER_REPAIR
NO_RUNTIME_CHANGE
```

## Goal

Repair `AudioFormatDescriptor` so it distinguishes valid signal bits from memory container bits.

This is required for formats such as signed 24-bit PCM carried in a 32-bit container.

## Natural family tree

```text
KivoAudioCoreCpp/
  include/
    kivo/core/contract/format/descriptor/
      sample_format.hpp
      audio_format_descriptor.hpp
  tests/
    contracts/format/descriptor/
      audio_format_descriptor_tests.cpp
```

## Scope

Allowed files:

```text
KivoAudioCoreCpp/include/kivo/core/contract/format/descriptor/audio_format_descriptor.hpp
KivoAudioCoreCpp/tests/contracts/format/descriptor/audio_format_descriptor_tests.cpp
KivoAudioCoreCpp/docs/tasks/p0_d_015_pcm_format_descriptor_container_consistency_task.md
```

## Acceptance criteria

```text
- Int24 uses 24 valid bits and 32 container bits.
- Stereo Int24 frame size is 8 bytes, not 6.
- Int16 with 24 valid bits is invalid.
- Container bits can be explicit or derived from SampleFormat metadata.
- Equality compares normalized container bits.
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-D-015-PCM-FORMAT-DESCRIPTOR-CONTAINER-CONSISTENCY
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
