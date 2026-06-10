# =============================================================================
# KivoAudioCoreCpp — CMakeLists.txt
# Phase: P0-B (Skeleton Governance, Configure-Only)
# =============================================================================
#
# P0-B Status: CONFIGURE-ONLY
# This file validates that CMake 3.24+ can parse the project structure.
# No targets, no runtime, no dependencies. P0-C will extend this file.
#
# Iron Rule 6: P0-B CMake can only be configure-only.
# Iron Rule 7: Configure pass is NOT build/test pass.
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

# --- P0-B: No targets below this line ---
# This line intentionally left as a guard. P0-C will add contract targets.
