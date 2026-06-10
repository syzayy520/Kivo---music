# Audio Truth Policy

**Purpose:** Define ground truth for audio format, channel layout, sample format, output mode, and resample decisions.  
**Scope:** All phases from P0-C onward.  
**Current Phase Rule:** P0-B has no audio runtime. This policy documents truth requirements.  
**Last Updated:** P0-B (V10.1)  

## 1. Purpose

This policy ensures all audio parameters are expressed as precise, verifiable truth — not approximations or marketing claims. It prevents sample rate/channel/format confusion and ensures bit-perfect eligibility is honest.

## 2. Scope

Applies to all future audio format types, channel layout types, sample format types, output mode types, and resample policy types.

## 3. Current Phase Rule (P0-B)

P0-B has no audio runtime. This policy documents the truth requirements for P0-C core contracts.

## 4. Allowed

- Documentation describing audio format requirements
- Policy documents referencing truth matrix
- Architecture diagrams showing format flow
- Precise audio_format struct (P0-C)
- Precise channel_layout struct (P0-C)
- Precise sample_format struct (P0-C)
- output_mode enum with eligibility matrix (P0-C)
- resample_decision contract (P0-C)

## 5. Forbidden

- Using only channel_count and sample_rate as "audio format"
- Using only bits_per_sample as "sample format"
- Declaring bit-perfect without eligibility check
- Declaring bit-perfect in shared mode without APO/spatial awareness
- Marketing claims about codec capability without legal review
- Treating resample as transparent or invisible
- Resample without decision reason documented

## 6. Truth Matrix Requirements

### 6.1 Audio Format Truth

Must include: sample_rate, channel_layout, sample_format, interleaving

### 6.2 Channel Layout Truth

Must express: named layout (stereo, 5.1, 7.1, etc.), channel count, channel mask  
Must NOT reduce to just channel_count

### 6.3 Sample Format Truth

Must express: valid_bits, container_bits, endianness, signedness, alignment, planar/interleaved  
Must NOT reduce to just bits_per_sample

### 6.4 Output Mode Truth

- Shared mode: default, NOT bit-perfect eligible
- Exclusive mode: bit-perfect eligible (pending device confirmation)
- Bit-perfect active: only when eligible + device confirmed

### 6.5 Resample Decision Truth

Every resample must have: source_format, target_format, reason, authority  
Reasons: device_capability, user_preference, format_mismatch, no_resample_possible

## 7. Gate / Check Method

- P0-C: contract tests validate truth struct completeness
- P0-H: device capability truth verification
- P0-M: bit-perfect eligibility verification

## 8. Failure Classification

- `AUDIO_FORMAT_INCOMPLETE_BLOCKER`: Missing required fields
- `CHANNEL_LAYOUT_REDUCED_BLOCKER`: Only channel_count used
- `SAMPLE_FORMAT_REDUCED_BLOCKER`: Only bits_per_sample used
- `BIT_PERFECT_FALSE_CLAIM_BLOCKER`: Bit-perfect declared without eligibility
- `RESAMPLE_UNDOCUMENTED_BLOCKER`: Resample without reason

## 9. Future Phase Ownership

- **P0-C:** Define truth contract types
- **P0-H:** Validate against real WASAPI device capabilities
- **P0-M:** Bit-perfect exclusive mode verification
- **P0-N:** Resampler integration with decision contract

## 10. Deferred Items

- DSD/DoP truth types (P0-Q)
- Hi-Res PCM truth types (P0-Q)
- Spatial audio metadata (future)

**Iron Rule 20:** Bit-perfect cannot be a marketing word; must be verifiable, rejectable, explainable.
**Iron Rule 21:** Shared mode default cannot declare bit-perfect; APO/spatial/enhancement/mixer/volume must enter truth report.
**Iron Rule 37:** Channel layout/sample format must be real semantics, not just count/rate.
**Iron Rule 38:** Sample format must express valid bits, container bits, endian, signedness, alignment, planar/interleaved.
