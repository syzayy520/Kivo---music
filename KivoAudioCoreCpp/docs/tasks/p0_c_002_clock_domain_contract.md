# P0-C-002: Clock Domain Contract (Full)

**Ticket ID:** KIVO-CPP-AUDIO-CORE-P0-C-002-CLOCK-DOMAIN  
**Phase:** P0-C  
**Type:** implementation / contract-definition  
**Execution Basis:** V10.1 FINAL P0-B READY LOCKED PATCHED + Enhancement Input Document §1.1  
**Prerequisite:** P0-C-001 COMPLETED, P0-012 COMPLETED  
**Date:** 2026-06-11  
**Status:** COMPLETED  

---

## 1. Ticket Objective

Expand the minimal clock domain contract from P0-C-001 into the full clock domain contract per Enhancement Input §1.1.

**P0-C-002 delivers:**
- Full clock domain types (device, stream, timeline)
- Position tracking types (decoded, rendered)
- Clock generation tracking
- Drift estimation types
- Clock policy enums (pause/resume, device lost, seek reset, drain, gapless)
- Comprehensive tests for all types

**P0-C-002 does NOT:**
- Implement real clock behavior
- Connect to WASAPI IAudioClock
- Implement drift recovery logic
- Implement gapless playback (P0-L prep only)

---

## 2. Scope Definition

### 2.1 Allowed Directories

| Category | Allowed Content |
|----------|-----------------|
| **Header files** | `include/kivo/core/contract/clock/` |
| **Test files** | `tests/contracts/clock/` |
| **CMake targets** | Update kivo_contract_tests |

### 2.2 Forbidden

| Category | Forbidden Content |
|----------|-------------------|
| **Runtime** | No real clock, no IAudioClock, no COM |
| **Platform** | No Windows API, no WASAPI |
| **State machine** | No state machine implementation |
| **External** | No third-party libraries |

---

## 3. Deliverables

### 3.1 Clock Domain Types

#### 3.1.1 DeviceClock

**Location:** `include/kivo/core/contract/clock/device_clock.hpp`

```cpp
namespace kivo::core::contract {

struct DeviceClock {
    SamplePosition position = kInvalidSamplePosition;
    GenerationId generation = GenerationId::initial();
    uint64_t timestamp_qpc = 0;  // Platform-neutral monotonic timestamp

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool operator==(const DeviceClock&) const noexcept = default;
};

} // namespace kivo::core::contract
```

#### 3.1.2 StreamClock

**Location:** `include/kivo/core/contract/clock/stream_clock.hpp`

```cpp
namespace kivo::core::contract {

struct StreamClock {
    SamplePosition position = kInvalidSamplePosition;
    GenerationId generation = GenerationId::initial();
    uint32_t sample_rate = 0;

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool operator==(const StreamClock&) const noexcept = default;
};

} // namespace kivo::core::contract
```

#### 3.1.3 TimelineClock

**Location:** `include/kivo/core/contract/clock/timeline_clock.hpp`

```cpp
namespace kivo::core::contract {

struct TimelineClock {
    SamplePosition position = kInvalidSamplePosition;
    GenerationId generation = GenerationId::initial();
    bool is_playing = false;

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool operator==(const TimelineClock&) const noexcept = default;
};

} // namespace kivo::core::contract
```

#### 3.1.4 DecodedPosition

**Location:** `include/kivo/core/contract/clock/decoded_position.hpp`

```cpp
namespace kivo::core::contract {

struct DecodedPosition {
    SamplePosition sample = kInvalidSamplePosition;
    FrameCount frames_decoded = 0;
    GenerationId generation = GenerationId::initial();

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool operator==(const DecodedPosition&) const noexcept = default;
};

} // namespace kivo::core::contract
```

#### 3.1.5 RenderedPosition

**Location:** `include/kivo/core/contract/clock/rendered_position.hpp`

```cpp
namespace kivo::core::contract {

struct RenderedPosition {
    SamplePosition sample = kInvalidSamplePosition;
    FrameCount frames_written = 0;
    GenerationId generation = GenerationId::initial();

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool operator==(const RenderedPosition&) const noexcept = default;
};

} // namespace kivo::core::contract
```

### 3.2 Drift Estimation

#### 3.2.1 DriftEstimate

**Location:** `include/kivo/core/contract/clock/drift_estimate.hpp`

```cpp
namespace kivo::core::contract {

enum class DriftDirection : uint8_t {
    Unknown = 0,
    None,       // Clocks are synchronized
    DeviceAhead,// Device clock is ahead of stream clock
    StreamAhead // Stream clock is ahead of device clock
};

struct DriftEstimate {
    DriftDirection direction = DriftDirection::Unknown;
    int64_t drift_samples = 0;  // Positive = device ahead, negative = stream ahead
    uint32_t confidence_pct = 0; // 0-100

    [[nodiscard]] bool is_valid() const noexcept;
    [[nodiscard]] bool is_significant(int64_t threshold) const noexcept;
    [[nodiscard]] bool operator==(const DriftEstimate&) const noexcept = default;
};

} // namespace kivo::core::contract
```

### 3.3 Clock Policy Enums

#### 3.3.1 PauseResumeFreezePolicy

**Location:** `include/kivo/core/contract/clock/pause_resume_freeze_policy.hpp`

```cpp
namespace kivo::core::contract {

enum class PauseResumeFreezePolicy : uint8_t {
    Unknown = 0,
    FreezeAll,      // All clocks freeze on pause, resume from frozen position
    FreezeDevice,   // Device clock freezes, stream clock continues
    FreezeTimeline, // Timeline freezes, device/stream continue
    None            // No freezing (clocks continue running)
};

} // namespace kivo::core::contract
```

#### 3.3.2 DeviceLostPositionReportRule

**Location:** `include/kivo/core/contract/clock/device_lost_position_report_rule.hpp`

```cpp
namespace kivo::core::contract {

enum class DeviceLostPositionReportRule : uint8_t {
    Unknown = 0,
    LastKnown,      // Report last known device position
    StreamEstimate, // Estimate from stream position
    Zero,           // Report zero
    Invalid         // Report invalid position
};

} // namespace kivo::core::contract
```

#### 3.3.3 SeekClockResetRule

**Location:** `include/kivo/core/contract/clock/seek_clock_reset_rule.hpp`

```cpp
namespace kivo::core::contract {

enum class SeekClockResetRule : uint8_t {
    Unknown = 0,
    ResetAll,       // Reset all clocks to seek target
    ResetDevice,    // Reset device clock only
    ResetStream,    // Reset stream clock only
    ResetTimeline,  // Reset timeline only
    IncrementGeneration // Increment generation without resetting
};

} // namespace kivo::core::contract
```

#### 3.3.4 DrainEosTimelineRule

**Location:** `include/kivo/core/contract/clock/drain_eos_timeline_rule.hpp`

```cpp
namespace kivo::core::contract {

enum class DrainEosTimelineRule : uint8_t {
    Unknown = 0,
    HoldAtEnd,      // Timeline holds at end position
    ResetToStart,   // Timeline resets to start
    ContinueRunning // Timeline continues past end
};

} // namespace kivo::core::contract
```

#### 3.3.5 GaplessTimelineContinuityRule

**Location:** `include/kivo/core/contract/clock/gapless_timeline_continuity_rule.hpp`

```cpp
namespace kivo::core::contract {

enum class GaplessTimelineContinuityRule : uint8_t {
    Unknown = 0,
    Continuous,     // Timeline continues seamlessly across tracks
    ResetPerTrack,  // Timeline resets for each track
    OffsetPerTrack  // Timeline applies offset per track
};

} // namespace kivo::core::contract
```

---

## 4. Test Requirements

### 4.1 DeviceClock Tests
- Default construction (invalid)
- Valid construction
- Equality comparison
- Invalid state (position = kInvalidSamplePosition)

### 4.2 StreamClock Tests
- Default construction (invalid)
- Valid construction
- Equality comparison
- Invalid state (sample_rate = 0)

### 4.3 TimelineClock Tests
- Default construction (invalid)
- Valid construction
- Equality comparison
- Playing state

### 4.4 DecodedPosition Tests
- Default construction (invalid)
- Valid construction
- Equality comparison
- Frame count tracking

### 4.5 RenderedPosition Tests
- Default construction (invalid)
- Valid construction
- Equality comparison
- Frame count tracking

### 4.6 DriftEstimate Tests
- Default construction (invalid)
- Valid construction
- Direction enum values
- Significance check
- Equality comparison

### 4.7 Policy Enum Tests
- All enum values for each policy
- Default value is Unknown
- Enum comparisons

---

## 5. Implementation Structure

### 5.1 File Layout

```
include/kivo/core/contract/clock/
├── clock_domain.hpp          (existing - ClockDomain, ClockSnapshot)
├── device_clock.hpp          (NEW)
├── stream_clock.hpp          (NEW)
├── timeline_clock.hpp        (NEW)
├── decoded_position.hpp      (NEW)
├── rendered_position.hpp     (NEW)
├── drift_estimate.hpp        (NEW)
├── pause_resume_freeze_policy.hpp      (NEW)
├── device_lost_position_report_rule.hpp (NEW)
├── seek_clock_reset_rule.hpp           (NEW)
├── drain_eos_timeline_rule.hpp         (NEW)
└── gapless_timeline_continuity_rule.hpp (NEW)

tests/contracts/clock/
├── clock_domain_tests.cpp    (existing - 7 tests)
├── device_clock_tests.cpp    (NEW)
├── stream_clock_tests.cpp    (NEW)
├── timeline_clock_tests.cpp  (NEW)
├── position_tests.cpp        (NEW - decoded + rendered)
├── drift_estimate_tests.cpp  (NEW)
└── clock_policy_tests.cpp    (NEW - all policy enums)
```

### 5.2 File Responsibilities

| File | Responsibility |
|------|---------------|
| `clock_domain.hpp` | ClockDomain enum, ClockSnapshot (existing) |
| `device_clock.hpp` | DeviceClock struct |
| `stream_clock.hpp` | StreamClock struct |
| `timeline_clock.hpp` | TimelineClock struct |
| `decoded_position.hpp` | DecodedPosition struct |
| `rendered_position.hpp` | RenderedPosition struct |
| `drift_estimate.hpp` | DriftDirection enum, DriftEstimate struct |
| `*_policy.hpp` | Policy enums (5 files) |

---

## 6. Acceptance Criteria

### 6.1 Functional Requirements
- [x] All clock domain types defined (DeviceClock, StreamClock, TimelineClock, DecodedPosition, RenderedPosition)
- [x] All policy enums defined (PauseResumeFreezePolicy, DeviceLostPositionReportRule, SeekClockResetRule, DrainEosTimelineRule, GaplessTimelineContinuityRule)
- [x] All tests pass (49 new tests, 92 total contract tests)
- [x] Platform neutrality maintained (no Windows/WASAPI/COM types)

### 6.2 Quality Requirements
- [x] No compiler warnings (/W4 /WX pass)
- [x] All files ≤200 lines
- [x] No empty files
- [x] Documentation complete (ticket spec)

### 6.3 Scope Requirements
- [x] No real clock implementation (data types only)
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
| 2026-06-11 | 1.1 | Implementation complete — 11 headers, 6 test files, 49 new tests, 92 total contract tests |

---

**Task Status:** COMPLETED  
**Completion Date:** 2026-06-11  
**Commit:** (pending)  
**Verification:** 92/92 contract tests + 68/68 capability tests PASS, 9/9 gates PASS  
**Next Action:** None — ready for commit  
**Responsible:** AI Agent  
