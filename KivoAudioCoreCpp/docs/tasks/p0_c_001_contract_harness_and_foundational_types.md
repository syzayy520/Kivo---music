# P0-C-001: Contract Harness and Foundational Types

**Ticket ID:** KIVO-CPP-AUDIO-CORE-P0-C-001-CONTRACT-HARNESS-AND-FOUNDATIONAL-TYPES  
**Phase:** P0-C  
**Type:** implementation / contract-definition / foundation  
**Execution Basis:** V10.1 FINAL P0-B READY LOCKED PATCHED + Enhancement Input Document  
**Prerequisite:** P0-C Phase Plan approved  
**Date:** 2026-06-10  
**Status:** COMPLETED  

---

## 1. Ticket Objective

Create the minimal compilable contract foundation for KivoAudioCoreCpp. This ticket establishes:
- Contract build/test harness
- Foundational types shared across all contract families
- First compilable contract code with tests

**P0-C-001 does NOT:**
- Implement real audio playback
- Connect WASAPI, FFmpeg, or mpv
- Create fake backends, runtime backends, or real playback harnesses
- Implement all 14 contract families (deferred to P0-C-002 through P0-C-014)

---

## 2. Scope Definition

### 2.1 Allowed Directories

These directories are **P0-C newly authorized scope**:

| Category | Allowed Content |
|----------|-----------------|
| **Source files** | `src/core/contract/` only |
| **Test files** | `tests/contracts/` only |
| **Header files** | `include/kivo/core/contract/` only |
| **CMake targets** | Contract library target, contract test target |
| **Dependencies** | Standard C++20 library only |

### 2.2 Allowed P0-B File Modifications

| File | Allowed Modification |
|------|---------------------|
| `CMakeLists.txt` | Add contract library/test targets |
| `CMakePresets.json` | Add contract build preset (if needed) |
| `tools/gates/` | Add/update P0-C contract gates |
| `docs/tasks/` | Task specifications |
| `docs/closeout/` | Phase closeout reports |

### 2.3 Forbidden

| Category | Forbidden Content |
|----------|-------------------|
| **Runtime** | No audio playback, no threads, no I/O |
| **Platform** | No WASAPI, no Windows API, no COM |
| **Decode** | No FFmpeg, no codec implementation |
| **Fake backends** | No fake implementations, no mocks |
| **State machine** | No state machine implementation |
| **Host integration** | No Tauri, no C ABI, no host callbacks |
| **External dependencies** | No third-party libraries |
| **Bucket directories** | No helper/utils/common/misc/glue/facade/types/defs |
| **Empty forests** | No empty contract directories |
| **Sample player** | No sample player, no demo |
| **Runtime executable** | No runtime binary, no playback executable |

---

## 3. Deliverables

### 3.1 Contract Build/Test Harness

**CMake targets:**
```cmake
# Contract library (header-only)
add_library(kivo_core_contract INTERFACE)
target_include_directories(kivo_core_contract INTERFACE include/)

# Contract tests
add_executable(kivo_contract_tests 
    tests/contracts/result_tests.cpp
    tests/contracts/generation_id_tests.cpp
    tests/contracts/sample_position_tests.cpp
    tests/contracts/frame_count_tests.cpp
    tests/contracts/audio_format_descriptor_tests.cpp
    tests/contracts/clock_domain_tests.cpp
    tests/contracts/seek_flush_tests.cpp
)
target_link_libraries(kivo_contract_tests PRIVATE kivo_core_contract)
target_compile_features(kivo_contract_tests PRIVATE cxx_std_20)
add_test(NAME contract_tests COMMAND kivo_contract_tests)
```

### 3.2 Foundational Types

#### 3.2.1 Result/Status Types

**Location:** `include/kivo/core/contract/result.hpp` / `src/core/contract/result.cpp`

```cpp
namespace kivo::core::contract {

enum class [[nodiscard]] Result {
    Ok,
    Error
};

template<typename T>
class [[nodiscard]] StatusOr {
public:
    // Constructors
    static StatusOr Ok(T value);
    static StatusOr Error(std::string_view message);
    
    // Observers
    bool is_ok() const noexcept;
    bool is_error() const noexcept;
    Result result() const noexcept;
    
    // Accessors
    const T& value() const;  // Precondition: is_ok()
    std::string_view error() const;  // Precondition: is_error()
    
private:
    Result result_;
    std::variant<T, std::string> data_;
};

} // namespace kivo::core::contract
```

#### 3.2.2 Generation ID

**Location:** `include/kivo/core/contract/generation_id.hpp`

```cpp
namespace kivo::core::contract {

class GenerationId {
public:
    static GenerationId initial() noexcept;
    static GenerationId next(GenerationId current) noexcept;
    
    bool operator==(const GenerationId&) const noexcept = default;
    auto operator<=>(const GenerationId&) const noexcept = default;
    
    uint64_t value() const noexcept;
    
private:
    explicit GenerationId(uint64_t id) noexcept;
    uint64_t id_;
};

} // namespace kivo::core::contract
```

#### 3.2.3 Sample Position / Frame Count

**Location:** `include/kivo/core/contract/sample_position.hpp`

```cpp
namespace kivo::core::contract {

using SamplePosition = uint64_t;
using FrameCount = uint64_t;

constexpr SamplePosition kInvalidSamplePosition = UINT64_MAX;
constexpr FrameCount kZeroFrameCount = 0;

} // namespace kivo::core::contract
```

#### 3.2.4 Audio Format Descriptor (Minimal)

**Location:** `include/kivo/core/contract/audio_format_descriptor.hpp`

```cpp
namespace kivo::core::contract {

enum class SampleFormat : uint8_t {
    Unknown = 0,
    Int16,
    Int24,
    Int32,
    Float32,
    Float64
};

enum class ChannelLayout : uint8_t {
    Unknown = 0,
    Mono,
    Stereo,
    Surround51,
    Surround71
};

struct AudioFormatDescriptor {
    SampleFormat sample_format = SampleFormat::Unknown;
    ChannelLayout channel_layout = ChannelLayout::Unknown;
    uint32_t sample_rate = 0;
    uint8_t bits_per_sample = 0;
    
    bool is_valid() const noexcept;
    bool operator==(const AudioFormatDescriptor&) const noexcept = default;
};

} // namespace kivo::core::contract
```

#### 3.2.5 Clock Domain (Minimal)

**Location:** `include/kivo/core/contract/clock_domain.hpp`

```cpp
namespace kivo::core::contract {

enum class ClockDomain : uint8_t {
    Unknown = 0,
    Device,      // Hardware sample clock
    Stream,      // Decoded stream logical clock
    Timeline     // User-facing playback position
};

struct ClockSnapshot {
    ClockDomain domain = ClockDomain::Unknown;
    SamplePosition position = kInvalidSamplePosition;
    GenerationId generation = GenerationId::initial();
    
    bool is_valid() const noexcept;
};

} // namespace kivo::core::contract
```

#### 3.2.6 Seek/Flush Minimal Enums

**Location:** `include/kivo/core/contract/seek_flush.hpp`

```cpp
namespace kivo::core::contract {

enum class SeekAccuracy : uint8_t {
    Exact,
    NearestKeyframe,
    NearestSample
};

enum class FlushScope : uint8_t {
    None = 0,
    Decoder = 1 << 0,
    Queue = 1 << 1,
    Renderer = 1 << 2,
    All = Decoder | Queue | Renderer
};

// Bitwise OR for FlushScope
constexpr FlushScope operator|(FlushScope a, FlushScope b) noexcept {
    return static_cast<FlushScope>(
        static_cast<uint8_t>(a) | static_cast<uint8_t>(b)
    );
}

constexpr bool has_flag(FlushScope scope, FlushScope flag) noexcept {
    return (static_cast<uint8_t>(scope) & static_cast<uint8_t>(flag)) != 0;
}

enum class DrainSemantics : uint8_t {
    None,
    DrainToZero,      // Play all remaining samples
    DrainToSilence,   // Play until silence threshold
    Immediate         // Stop immediately
};

} // namespace kivo::core::contract
```

---

## 4. Test Requirements

### 4.1 Result/Status Tests

- Result enum values
- StatusOr construction (Ok, Error)
- StatusOr observers (is_ok, is_error, result)
- StatusOr accessors (value, error)
- StatusOr precondition violations

### 4.2 Generation ID Tests

- Initial generation
- Next generation
- Equality comparison
- Ordering comparison
- Value access

### 4.3 Sample Position / Frame Count Tests

- Type aliases
- Invalid position constant
- Zero frame count constant

### 4.4 Audio Format Descriptor Tests

- SampleFormat enum values
- ChannelLayout enum values
- AudioFormatDescriptor construction
- AudioFormatDescriptor validity check
- AudioFormatDescriptor equality

### 4.5 Clock Domain Tests

- ClockDomain enum values
- ClockSnapshot construction
- ClockSnapshot validity check

### 4.6 Seek/Flush Tests

- SeekAccuracy enum values
- FlushScope enum values
- FlushScope bitwise operations
- DrainSemantics enum values

---

## 5. Implementation Structure

### 5.1 Directory Layout

```
KivoAudioCoreCpp/
├── include/
│   └── kivo/
│       └── core/
│           └── contract/
│               ├── result.hpp
│               ├── generation_id.hpp
│               ├── sample_position.hpp
│               ├── frame_count.hpp
│               ├── audio_format_descriptor.hpp
│               ├── clock_domain.hpp
│               └── seek_flush.hpp
├── src/
│   └── core/
│       └── contract/
│           └── result.cpp
└── tests/
    └── contracts/
        ├── result_tests.cpp
        ├── generation_id_tests.cpp
        ├── sample_position_tests.cpp
        ├── frame_count_tests.cpp
        ├── audio_format_descriptor_tests.cpp
        ├── clock_domain_tests.cpp
        └── seek_flush_tests.cpp
```

### 5.2 File Responsibilities

| File | Responsibility |
|------|---------------|
| `result.hpp` | Result enum, StatusOr template |
| `generation_id.hpp` | GenerationId class |
| `sample_position.hpp` | SamplePosition type alias |
| `frame_count.hpp` | FrameCount type alias |
| `audio_format_descriptor.hpp` | SampleFormat, ChannelLayout, AudioFormatDescriptor |
| `clock_domain.hpp` | ClockDomain enum, ClockSnapshot |
| `seek_flush.hpp` | SeekAccuracy, FlushScope, DrainSemantics |
| `result.cpp` | StatusOr implementation |
| `*_tests.cpp` | Corresponding test files |

---

## 6. Acceptance Criteria

### 6.1 Functional Requirements

- [ ] Contract build harness compiles
- [ ] Contract test harness runs
- [ ] All foundational types defined
- [ ] All tests pass
- [ ] Platform neutrality maintained

### 6.2 Quality Requirements

- [ ] No compiler warnings
- [ ] Code follows .clang-format style
- [ ] Documentation complete for all public APIs
- [ ] 100% test coverage for foundational types

### 6.3 Scope Requirements

- [ ] No runtime implementation
- [ ] No platform-specific code
- [ ] No external dependencies
- [ ] No fake backends
- [ ] No WASAPI/FFmpeg/mpv

---

## 7. Gate Validation

All P0-B gates must pass:
- Forbidden Token Gate: PASS
- Genealogy Gate: PASS
- No Empty Forest Gate: PASS
- No Audio Runtime Gate: PASS
- Policy Substance Gate: PASS
- Dependency/License Gate: PASS
- Toolchain/Configure Gate: PASS

---

## 8. Risk Assessment

| Risk | Likelihood | Impact | Mitigation |
|------|------------|--------|------------|
| CMake integration complexity | Medium | Medium | Start with INTERFACE library |
| Test framework setup | Low | Low | Use Catch2 or simple main() |
| Type design iteration | Medium | Medium | Start minimal, expand later |
| Scope creep | Low | High | Strict adherence to deliverables |

---

## 9. Post-P0-C-001 Handoff

P0-C-002 (Clock Domain Contract Full) depends on:
- All foundational types from P0-C-001
- Contract build/test harness from P0-C-001
- GenerationId from P0-C-001

---

## 10. Appendix

### 10.1 References

- V10.1 FINAL P0-B READY LOCKED PATCHED
- P0-C Phase Plan
- P0-C through P0-I Enhancement Input Document

### 10.2 Change History

| Date | Version | Description |
|------|---------|-------------|
| 2026-06-10 | 1.0 | Initial ticket specification |

---

**Task Status:** COMPLETED  
**Completion Date:** 2026-06-10  
**Commit:** 70392ad  
**Branch:** master  
**Remote HEAD:** 70392ad0d613874076e13fdbbb06d3528f30a33c
