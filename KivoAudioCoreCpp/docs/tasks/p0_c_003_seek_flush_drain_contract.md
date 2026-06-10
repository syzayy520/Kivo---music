# P0-C-003: Seek/Flush/Drain Contract (Full)

**Ticket ID:** KIVO-CPP-AUDIO-CORE-P0-C-003-SEEK-FLUSH-DRAIN  
**Phase:** P0-C  
**Type:** implementation / contract-definition  
**Execution Basis:** V10.1 FINAL P0-B READY LOCKED PATCHED + Enhancement Input Document §1.2  
**Prerequisite:** P0-C-001 COMPLETED, P0-C-002 COMPLETED  
**Date:** 2026-06-11  
**Status:** COMPLETED  

---

## 1. Ticket Objective

Expand the minimal seek/flush/drain contract from P0-C-001 into the full seek/flush/drain contract per Enhancement Input §1.2.

**P0-C-003 delivers:**
- Seek intent types (user's seek request)
- Seek target types (sample-accurate position)
- Seek accuracy (existing)
- Seek generation tracking
- Flush scope types (existing)
- Drain semantics (existing)
- Seek/flush policy enums
- Comprehensive tests for all types

**P0-C-003 does NOT:**
- Implement real seek behavior
- Connect to FFmpeg seeking
- Implement state machine transitions
- Implement anti-glitch policies

---

## 2. Scope Definition

### 2.1 Allowed Directories

| Category | Allowed Content |
|----------|-----------------|
| **Header files** | `include/kivo/core/contract/seek/` |
| **Test files** | `tests/contracts/seek/` |
| **CMake targets** | Update kivo_contract_tests |

### 2.2 Forbidden

| Category | Forbidden Content |
|----------|-------------------|
| **Runtime** | No real seek, no FFmpeg, no mpv |
| **Platform** | No Windows API, no WASAPI |
| **State machine** | No state machine implementation |
| **External** | No third-party libraries |

---

## 3. Deliverables

### 3.1 Seek Intent Types

#### 3.1.1 SeekIntent

**Location:** `include/kivo/core/contract/seek/seek_intent.hpp`

```cpp
namespace kivo::core::contract {

enum class SeekOrigin : uint8_t {
    Absolute,     // Absolute position from start
    Relative,     // Relative to current position
    RelativeToEnd // Relative to end of stream
};

struct SeekIntent {
    int64_t offset = 0;        // Sample offset (interpretation depends on origin)
    SeekOrigin origin = SeekOrigin::Absolute;
    bool request_flush = true; // Whether to flush pipeline after seek

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool operator==(const SeekIntent&) const noexcept = default;
};

} // namespace kivo::core::contract
```

### 3.2 Seek Target Types

#### 3.2.1 SeekTarget

**Location:** `include/kivo/core/contract/seek/seek_target.hpp`

```cpp
namespace kivo::core::contract {

struct SeekTarget {
    SamplePosition position = kInvalidSamplePosition;
    GenerationId generation = GenerationId::initial();
    SeekAccuracy accuracy = SeekAccuracy::Exact;

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool operator==(const SeekTarget&) const noexcept = default;
};

} // namespace kivo::core::contract
```

### 3.3 Seek Generation

#### 3.3.1 SeekGeneration

**Location:** `include/kivo/core/contract/seek/seek_generation.hpp`

```cpp
namespace kivo::core::contract {

struct SeekGeneration {
    GenerationId generation = GenerationId::initial();
    uint32_t seek_count = 0;     // Total seeks in this session
    uint64_t last_seek_qpc = 0;  // Timestamp of last seek

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool operator==(const SeekGeneration&) const noexcept = default;
};

} // namespace kivo::core::contract
```

### 3.4 Seek Policy Enums

#### 3.4.1 SeekAntiGlitchPolicy

**Location:** `include/kivo/core/contract/seek/seek_anti_glitch_policy.hpp`

```cpp
namespace kivo::core::contract {

enum class SeekAntiGlitchPolicy : uint8_t {
    Unknown = 0,
    MuteDuringSeek,    // Mute output during seek
    FadeOutFadeIn,     // Fade out before, fade in after
    Crossfade,         // Crossfade old and new audio
    None               // No anti-glitch (may cause pops)
};

} // namespace kivo::core::contract
```

#### 3.4.2 StaleFrameDiscardRule

**Location:** `include/kivo/core/contract/seek/stale_frame_discard_rule.hpp`

```cpp
namespace kivo::core::contract {

enum class StaleFrameDiscardRule : uint8_t {
    Unknown = 0,
    DiscardAll,        // Discard all frames from old generation
    DiscardAfterSeek,  // Only discard frames after seek position
    KeepIfDecoded      // Keep frames already decoded
};

} // namespace kivo::core::contract
```

---

## 4. Test Requirements

### 4.1 SeekIntent Tests
- Default construction (invalid)
- Valid construction (absolute, relative, relative-to-end)
- Equality comparison
- Invalid state (zero offset for absolute)

### 4.2 SeekTarget Tests
- Default construction (invalid)
- Valid construction
- Equality comparison
- Accuracy enum integration

### 4.3 SeekGeneration Tests
- Default construction (invalid)
- Valid construction
- Equality comparison
- Seek count tracking

### 4.4 Seek Policy Tests
- All enum values for each policy
- Default value is Unknown
- Enum comparisons

---

## 5. Implementation Structure

### 5.1 File Layout

```
include/kivo/core/contract/seek/
├── seek_flush.hpp              (existing - SeekAccuracy, FlushScope, DrainSemantics)
├── seek_intent.hpp             (NEW - SeekOrigin, SeekIntent)
├── seek_target.hpp             (NEW - SeekTarget)
├── seek_generation.hpp         (NEW - SeekGeneration)
├── seek_anti_glitch_policy.hpp (NEW - SeekAntiGlitchPolicy)
└── stale_frame_discard_rule.hpp (NEW - StaleFrameDiscardRule)

tests/contracts/seek/
├── seek_flush_tests.cpp        (existing - 7 tests)
├── seek_intent_tests.cpp       (NEW)
├── seek_target_tests.cpp       (NEW)
├── seek_generation_tests.cpp   (NEW)
└── seek_policy_tests.cpp       (NEW - all policy enums)
```

### 5.2 File Responsibilities

| File | Responsibility |
|------|---------------|
| `seek_flush.hpp` | SeekAccuracy, FlushScope, DrainSemantics (existing) |
| `seek_intent.hpp` | SeekOrigin enum, SeekIntent struct |
| `seek_target.hpp` | SeekTarget struct |
| `seek_generation.hpp` | SeekGeneration struct |
| `seek_anti_glitch_policy.hpp` | SeekAntiGlitchPolicy enum |
| `stale_frame_discard_rule.hpp` | StaleFrameDiscardRule enum |

---

## 6. Acceptance Criteria

### 6.1 Functional Requirements
- [x] All seek intent/target types defined (SeekIntent, SeekTarget, SeekGeneration)
- [x] All policy enums defined (SeekAntiGlitchPolicy, StaleFrameDiscardRule)
- [x] All tests pass (30 new tests, 122 total contract tests)
- [x] Platform neutrality maintained (no Windows/WASAPI/COM types)

### 6.2 Quality Requirements
- [x] No compiler warnings (/W4 /WX pass)
- [x] All files ≤200 lines
- [x] No empty files
- [x] Documentation complete (ticket spec)

### 6.3 Scope Requirements
- [x] No real seek implementation (data types only)
- [x] No Windows-specific code (platform-neutral)
- [x] No external dependencies (header-only)

---

## 7. Gate Validation

All P0-B gates must pass:
- Forbidden Token Gate: PASS
- Genealogy Gate: PASS
- Contract Genealogy Gate: PASS
- No Empty Forest Gate: PASS
- No Audio Runtime Gate: PASS
- Policy Substance Gate: PASS
- Dependency/License Gate: PASS
- Toolchain/Configure Gate: PASS
- Audio Core Scope Lock Gate: PASS

---

## 8. Change History

| Date | Version | Description |
|------|---------|-------------|
| 2026-06-11 | 1.0 | Initial ticket specification |
| 2026-06-11 | 1.1 | Implementation complete — 5 new headers, 4 new test files, 30 new tests, 122 total contract tests |

---

**Task Status:** COMPLETED  
**Completion Date:** 2026-06-11  
**Verification:** 122/122 contract tests PASS, 9/9 gates PASS  
**Next Action:** None — ready for commit  
**Responsible:** AI Agent  
