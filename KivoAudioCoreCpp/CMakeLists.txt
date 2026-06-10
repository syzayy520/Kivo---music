# =============================================================================
# KivoAudioCoreCpp — CMakeLists.txt
# Phase: P0-C (Contract Harness and Foundational Types)
# =============================================================================
#
# P0-C Status: CONTRACT BUILD/TEST
# Header-only contract library + contract test executable.
# No runtime dependencies. No platform code. No third-party libraries.
#
# Iron Rule 6: P0-C CMake targets contract types only.
# Iron Rule 7: Contract tests validate type semantics, not runtime behavior.
# =============================================================================

cmake_minimum_required(VERSION 3.24)

project(KivoAudioCoreCpp
    VERSION 0.0.0
    LANGUAGES CXX
    DESCRIPTION "Kivo Audio Core — C++ Windows Desktop Audio Engine"
)

# --- C++ Standard Policy ---
# C++20 is required. No extensions. No fallback.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- MSVC / Windows x64 Policy ---
# Future phases will enforce:
#   - MSVC 19.34+ (VS 2022 17.4+)
#   - Windows x64 target architecture
#   - /W4 warning baseline
#   - _WIN32_WINNT >= 0x0A00 (Windows 10+)
# These are documented in docs/architecture/toolchain_and_build_policy.md
# but NOT enforced here in P0-B (configure-only).

# =============================================================================
# P0-C: Contract Build/Test Harness
# =============================================================================
# Header-only contract library
add_library(kivo_core_contract INTERFACE)
target_include_directories(kivo_core_contract INTERFACE include/)
target_compile_features(kivo_core_contract INTERFACE cxx_std_20)

# Contract tests
add_executable(kivo_contract_tests
    tests/contracts/contract_tests_main.cpp
    tests/contracts/foundation/result_tests.cpp
    tests/contracts/foundation/generation_id_tests.cpp
    tests/contracts/foundation/sample_position_tests.cpp
    tests/contracts/format/audio_format_descriptor_tests.cpp
    tests/contracts/clock/clock_domain_tests.cpp
    tests/contracts/seek/seek_flush_tests.cpp
)
target_link_libraries(kivo_contract_tests PRIVATE kivo_core_contract)
target_compile_features(kivo_contract_tests PRIVATE cxx_std_20)
target_compile_options(kivo_contract_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Register with CTest
enable_testing()
add_test(NAME contract_tests COMMAND kivo_contract_tests)
