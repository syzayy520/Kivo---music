# =============================================================================
# KivoAudioCoreCpp — CMakeLists.txt
# Phase: P0-D (Validation Hardening and Contract Correctness Repair)
# =============================================================================
#
# P0-D Status: CONTRACT BUILD/TEST + VALIDATION HARDENING
# Header-only contract library + contract test executable.
# No runtime dependencies. No platform code. No third-party libraries.
#
# Iron Rule 6: P0-D CMake targets contract types and validation-only tests.
# Iron Rule 7: Contract tests validate type semantics and public header composability, not runtime behavior.
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
# These are documented in docs/architecture/toolchain_and_build_policy.md.

# =============================================================================
# P0-D: Contract Build/Test Harness
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
    tests/contracts/composability/public_header_composability_tests.cpp
    tests/contracts/format/descriptor/audio_format_descriptor_tests.cpp
    tests/contracts/format/descriptor/sample_format_tests.cpp
    tests/contracts/format/descriptor/channel_layout_tests.cpp
    tests/contracts/format/descriptor/frame_layout_tests.cpp
    tests/contracts/format/descriptor/channel_mask_tests.cpp
    tests/contracts/format/roles/native_decoded_format_tests.cpp
    tests/contracts/format/roles/core_canonical_format_tests.cpp
    tests/contracts/format/roles/render_format_tests.cpp
    tests/contracts/format/roles/device_format_tests.cpp
    tests/contracts/format/negotiation/format_negotiation_result_tests.cpp
    tests/contracts/format/negotiation/conversion_policy_tests.cpp
    tests/contracts/format/negotiation/resample_decision_tests.cpp
    tests/contracts/format/negotiation/bit_perfect_eligibility_tests.cpp
    tests/contracts/format/negotiation/negotiated_output_mode_tests.cpp
    tests/contracts/clock/domain/clock_domain_tests.cpp
    tests/contracts/clock/domain/device_clock_tests.cpp
    tests/contracts/clock/domain/stream_clock_tests.cpp
    tests/contracts/clock/domain/timeline_clock_tests.cpp
    tests/contracts/clock/position/decoded_position_tests.cpp
    tests/contracts/clock/position/rendered_position_tests.cpp
    tests/contracts/clock/drift/drift_estimate_tests.cpp
    tests/contracts/clock/policy/pause_resume_freeze_policy_tests.cpp
    tests/contracts/clock/policy/device_lost_position_report_rule_tests.cpp
    tests/contracts/clock/policy/seek_clock_reset_rule_tests.cpp
    tests/contracts/clock/policy/drain_eos_timeline_rule_tests.cpp
    tests/contracts/clock/policy/gapless_timeline_continuity_rule_tests.cpp
    tests/contracts/seek/flush/seek_flush_tests.cpp
    tests/contracts/seek/intent/seek_intent_tests.cpp
    tests/contracts/seek/target/seek_target_tests.cpp
    tests/contracts/seek/generation/seek_generation_tests.cpp
    tests/contracts/seek/policy/seek_anti_glitch_policy_tests.cpp
    tests/contracts/seek/policy/stale_frame_discard_rule_tests.cpp
    tests/contracts/output/truth/output_truth_snapshot_tests.cpp
    tests/contracts/output/truth/bitperfect_truth_report_tests.cpp
    tests/contracts/output/truth/bitperfect_rejection_reason_tests.cpp
    tests/contracts/output/processing/processing_participation_tests.cpp
    tests/contracts/output/path/device_format_match_tests.cpp
    tests/contracts/output/path/sample_transparency_tests.cpp
    tests/contracts/output/path/host_audio_engine_participation_tests.cpp
    tests/contracts/output/path/output_path_truth_tests.cpp
    tests/contracts/output/decision/output_policy_decision_tests.cpp
    tests/contracts/output/decision/output_policy_explanation_tests.cpp
    tests/contracts/realtime/path/realtime_path_contract_tests.cpp
    tests/contracts/realtime/prohibition/realtime_prohibition_contract_tests.cpp
    tests/contracts/realtime/ownership/realtime_ownership_contract_tests.cpp
    tests/contracts/realtime/buffer/realtime_buffer_contract_tests.cpp
    tests/contracts/realtime/transport/realtime_transport_contract_tests.cpp
    tests/contracts/realtime/boundary/realtime_boundary_contract_tests.cpp
    tests/contracts/generation/identity/generation_identity_tests.cpp
    tests/contracts/generation/comparison/generation_comparison_tests.cpp
    tests/contracts/generation/classification/generation_classification_tests.cpp
    tests/contracts/generation/advance/generation_advance_tests.cpp
    tests/contracts/buffer/identity/buffer_identity_tests.cpp
    tests/contracts/buffer/ownership/buffer_ownership_tests.cpp
    tests/contracts/buffer/lifetime/buffer_lifetime_tests.cpp
    tests/contracts/buffer/reference/buffer_reference_tests.cpp
    tests/contracts/buffer/share/buffer_share_tests.cpp
    tests/contracts/queue/capacity/queue_capacity_tests.cpp
    tests/contracts/queue/slot/queue_slot_tests.cpp
    tests/contracts/queue/staleness/queue_staleness_tests.cpp
    tests/contracts/queue/boundary/queue_boundary_tests.cpp
    tests/contracts/source/identity/source_identity_tests.cpp
    tests/contracts/source/location/source_location_tests.cpp
    tests/contracts/source/access/source_access_tests.cpp
    tests/contracts/source/reader/source_reader_tests.cpp
    tests/contracts/source/container/source_container_tests.cpp
    tests/contracts/source/boundary/source_boundary_tests.cpp
    tests/contracts/source/outcome/source_outcome_tests.cpp
    tests/contracts/cue/boundary/cue_boundary_tests.cpp
    tests/contracts/cue/range/cue_range_tests.cpp
    tests/contracts/cue/continuity/cue_continuity_tests.cpp
    tests/contracts/error/domain/error_domain_tests.cpp
    tests/contracts/error/domain/error_domain_category_tests.cpp
    tests/contracts/error/severity/error_severity_tests.cpp
    tests/contracts/error/severity/error_attributes_tests.cpp
    tests/contracts/error/recovery/recovery_action_tests.cpp
    tests/contracts/error/recovery/recovery_matrix_tests.cpp
    tests/contracts/error/outcome/error_outcome_tests.cpp
    tests/contracts/state/phase/state_phase_tests.cpp
    tests/contracts/state/transition/state_transition_tests.cpp
    tests/contracts/state/rejection/state_rejection_tests.cpp
    tests/contracts/state/rule/state_rule_tests.cpp
    tests/contracts/state/scenario/state_scenario_tests.cpp
    tests/contracts/state/contract/state_machine_contract_tests.cpp
    tests/contracts/command/identity/command_identity_tests.cpp
    tests/contracts/command/kind/command_kind_tests.cpp
    tests/contracts/command/ordering/command_ordering_tests.cpp
    tests/contracts/command/decision/command_decision_tests.cpp
    tests/contracts/command/scenario/command_scenario_tests.cpp
    tests/contracts/command/contract/command_semantics_contract_tests.cpp
    tests/contracts/observability/event/observability_event_tests.cpp
    tests/contracts/observability/metric/observability_metric_tests.cpp
    tests/contracts/observability/trace/observability_trace_tests.cpp
    tests/contracts/observability/diagnostic/observability_diagnostic_tests.cpp
    tests/contracts/observability/contract/observability_contract_tests.cpp
    tests/contracts/policy/privacy/privacy_policy_tests.cpp
    tests/contracts/policy/distribution/distribution_policy_tests.cpp
    tests/contracts/policy/contract/audio_core_policy_contract_tests.cpp
)
target_link_libraries(kivo_contract_tests PRIVATE kivo_core_contract)
target_compile_features(kivo_contract_tests PRIVATE cxx_std_20)
target_compile_options(kivo_contract_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# =============================================================================
# P0-012: Capability Negotiation Foundation
# =============================================================================
# Capability contract tests (separate executable)
add_executable(kivo_capability_tests
    tests/contracts/capability/capability_tests_main.cpp
    tests/contracts/capability/identity_tests.cpp
    tests/contracts/capability/quality_tests.cpp
    tests/contracts/capability/constraint_tests.cpp
    tests/contracts/capability/component_tests.cpp
    tests/contracts/capability/domain_tests.cpp
    tests/contracts/capability/negotiation_tests.cpp
)
target_link_libraries(kivo_capability_tests PRIVATE kivo_core_contract)
target_compile_features(kivo_capability_tests PRIVATE cxx_std_20)
target_compile_options(kivo_capability_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Register with CTest
enable_testing()
add_test(NAME contract_tests COMMAND kivo_contract_tests)
add_test(NAME capability_tests COMMAND kivo_capability_tests)
