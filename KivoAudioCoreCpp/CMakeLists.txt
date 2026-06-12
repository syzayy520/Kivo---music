# =============================================================================
# KivoAudioCoreCpp — CMakeLists.txt
# Phase: P0-D/P0-E (Deterministic Render Proof)
# =============================================================================
#
# P0-D/P0-E Status: RENDER BOUNDARY + DETERMINISTIC FAKE BACKEND
# Platform-neutral public boundaries plus bounded deterministic test runtime.
# No production device code. No platform API. No third-party libraries.
#
# Boundary targets remain platform-neutral and validation-only.
# Production runtime implementation begins with the WASAPI adapter milestone.
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

# Platform-neutral render boundary
add_library(kivo_core_render INTERFACE)
target_include_directories(kivo_core_render INTERFACE include/)
target_compile_features(kivo_core_render INTERFACE cxx_std_20)
target_link_libraries(kivo_core_render INTERFACE kivo_core_contract)

# Platform-neutral decode boundary and local byte source
add_library(kivo_decode_boundary STATIC
    src/source/local/local_file_byte_source.cpp
)
target_link_libraries(kivo_decode_boundary PUBLIC kivo_core_contract)
target_include_directories(kivo_decode_boundary PUBLIC include/)
target_compile_features(kivo_decode_boundary PUBLIC cxx_std_20)
target_compile_options(kivo_decode_boundary PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Optional FFmpeg adapter. The SDK is external, validated, LGPL shared, and
# never downloaded by configure or build.
set(KIVO_FFMPEG_ROOT "$ENV{KIVO_FFMPEG_ROOT}" CACHE PATH
    "Root of the approved FFmpeg 7.1 LGPL shared SDK")

if(KIVO_FFMPEG_ROOT)
    if(NOT WIN32)
        message(FATAL_ERROR "The approved KIVO_FFMPEG_ROOT baseline is Windows x64.")
    endif()

    set(KIVO_FFMPEG_INCLUDE_DIR "${KIVO_FFMPEG_ROOT}/include")
    set(KIVO_FFMPEG_LIBRARY_DIR "${KIVO_FFMPEG_ROOT}/lib")
    set(KIVO_FFMPEG_BINARY_DIR "${KIVO_FFMPEG_ROOT}/bin")
    set(KIVO_FFMPEG_REQUIRED_FILES
        "${KIVO_FFMPEG_INCLUDE_DIR}/libavcodec/avcodec.h"
        "${KIVO_FFMPEG_INCLUDE_DIR}/libavformat/avformat.h"
        "${KIVO_FFMPEG_INCLUDE_DIR}/libavutil/avutil.h"
        "${KIVO_FFMPEG_INCLUDE_DIR}/libswresample/swresample.h"
        "${KIVO_FFMPEG_LIBRARY_DIR}/avcodec.lib"
        "${KIVO_FFMPEG_LIBRARY_DIR}/avformat.lib"
        "${KIVO_FFMPEG_LIBRARY_DIR}/avutil.lib"
        "${KIVO_FFMPEG_LIBRARY_DIR}/swresample.lib"
        "${KIVO_FFMPEG_BINARY_DIR}/avcodec-61.dll"
        "${KIVO_FFMPEG_BINARY_DIR}/avformat-61.dll"
        "${KIVO_FFMPEG_BINARY_DIR}/avutil-59.dll"
        "${KIVO_FFMPEG_BINARY_DIR}/swresample-5.dll"
        "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
    )
    foreach(required_file IN LISTS KIVO_FFMPEG_REQUIRED_FILES)
        if(NOT EXISTS "${required_file}")
            message(FATAL_ERROR
                "KIVO_FFMPEG_ROOT is incomplete or has the wrong ABI: ${required_file}")
        endif()
    endforeach()

    function(kivo_import_ffmpeg_library target_name import_library runtime_dll)
        add_library(${target_name} SHARED IMPORTED GLOBAL)
        set_target_properties(${target_name} PROPERTIES
            IMPORTED_IMPLIB "${KIVO_FFMPEG_LIBRARY_DIR}/${import_library}"
            IMPORTED_LOCATION "${KIVO_FFMPEG_BINARY_DIR}/${runtime_dll}"
            INTERFACE_INCLUDE_DIRECTORIES "${KIVO_FFMPEG_INCLUDE_DIR}"
        )
    endfunction()

    kivo_import_ffmpeg_library(
        kivo_ffmpeg_avcodec avcodec.lib avcodec-61.dll)
    kivo_import_ffmpeg_library(
        kivo_ffmpeg_avformat avformat.lib avformat-61.dll)
    kivo_import_ffmpeg_library(
        kivo_ffmpeg_avutil avutil.lib avutil-59.dll)
    kivo_import_ffmpeg_library(
        kivo_ffmpeg_swresample swresample.lib swresample-5.dll)

    add_library(kivo_ffmpeg_decode_adapter STATIC
        src/adapters/ffmpeg/source/ffmpeg_avio_bridge.cpp
        src/adapters/ffmpeg/container/ffmpeg_demux_session.cpp
        src/adapters/ffmpeg/codec/ffmpeg_audio_decoder.cpp
        src/adapters/ffmpeg/conversion/ffmpeg_audio_converter.cpp
        src/adapters/ffmpeg/mapping/ffmpeg_audio_format.cpp
        src/adapters/ffmpeg/ffmpeg_audio_decode_session.cpp
    )
    target_link_libraries(kivo_ffmpeg_decode_adapter
        PUBLIC kivo_decode_boundary
        PRIVATE
            kivo_ffmpeg_avformat
            kivo_ffmpeg_avcodec
            kivo_ffmpeg_swresample
            kivo_ffmpeg_avutil
    )
    target_include_directories(kivo_ffmpeg_decode_adapter
        PUBLIC include/
        PRIVATE src/adapters/ffmpeg
    )
    target_compile_features(kivo_ffmpeg_decode_adapter PUBLIC cxx_std_20)
    target_compile_options(kivo_ffmpeg_decode_adapter PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )

    set(KIVO_FFMPEG_FIXTURE_DIR
        "${CMAKE_CURRENT_BINARY_DIR}/fixtures/ffmpeg")
    set(KIVO_FFMPEG_WAVE_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/tone_pcm_s16le.wav")
    set(KIVO_FFMPEG_FLAC_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/tone.flac")
    set(KIVO_FFMPEG_MP3_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/tone.mp3")
    add_custom_command(
        OUTPUT
            "${KIVO_FFMPEG_WAVE_FIXTURE}"
            "${KIVO_FFMPEG_FLAC_FIXTURE}"
            "${KIVO_FFMPEG_MP3_FIXTURE}"
        COMMAND "${CMAKE_COMMAND}" -E make_directory
            "${KIVO_FFMPEG_FIXTURE_DIR}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=523.25:duration=2:sample_rate=44100"
            -ac 2 -c:a pcm_s16le "${KIVO_FFMPEG_WAVE_FIXTURE}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=659.25:duration=2:sample_rate=44100"
            -ac 2 -c:a flac "${KIVO_FFMPEG_FLAC_FIXTURE}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=783.99:duration=2:sample_rate=44100"
            -ac 2 -c:a libmp3lame -b:a 192k "${KIVO_FFMPEG_MP3_FIXTURE}"
        VERBATIM
    )
    add_custom_target(kivo_ffmpeg_test_fixtures
        DEPENDS
            "${KIVO_FFMPEG_WAVE_FIXTURE}"
            "${KIVO_FFMPEG_FLAC_FIXTURE}"
            "${KIVO_FFMPEG_MP3_FIXTURE}"
    )

    add_executable(kivo_ffmpeg_decode_tests
        tests/ffmpeg_decode/ffmpeg_decode_tests_main.cpp
        tests/ffmpeg_decode/ffmpeg_session_tests.cpp
    )
    target_link_libraries(kivo_ffmpeg_decode_tests
        PRIVATE kivo_ffmpeg_decode_adapter)
    target_compile_features(kivo_ffmpeg_decode_tests PRIVATE cxx_std_20)
    target_compile_options(kivo_ffmpeg_decode_tests PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
    add_dependencies(kivo_ffmpeg_decode_tests kivo_ffmpeg_test_fixtures)

    foreach(ffmpeg_dll
        avcodec-61.dll
        avformat-61.dll
        avutil-59.dll
        swresample-5.dll)
        add_custom_command(TARGET kivo_ffmpeg_decode_tests POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${KIVO_FFMPEG_BINARY_DIR}/${ffmpeg_dll}"
                "$<TARGET_FILE_DIR:kivo_ffmpeg_decode_tests>"
            VERBATIM
        )
    endforeach()
endif()

# Bounded SPSC render transport
add_library(kivo_render_queue STATIC
    src/core/render/queue/spsc_audio_block_queue.cpp
    src/core/render/pump/render_pump.cpp
)
target_link_libraries(kivo_render_queue PUBLIC kivo_core_render)
target_include_directories(kivo_render_queue PUBLIC include/)
target_compile_features(kivo_render_queue PUBLIC cxx_std_20)
target_compile_options(kivo_render_queue PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Serialized product-control session state
add_library(kivo_playback_session STATIC
    src/core/playback/recovery/playback_recovery_executor.cpp
    src/core/playback/session/command/playback_session_command.cpp
    src/core/playback/session/controller/playback_session_controller.cpp
    src/core/playback/session/position/playback_session_position.cpp
    src/core/playback/session/recovery/playback_session_recovery.cpp
    src/core/playback/session/result/playback_session_result.cpp
    src/core/playback/session/seek/playback_session_seek.cpp
    src/core/playback/session/snapshot/playback_session_snapshot.cpp
)
target_link_libraries(kivo_playback_session PUBLIC kivo_core_contract)
target_include_directories(kivo_playback_session PUBLIC include/)
target_compile_features(kivo_playback_session PUBLIC cxx_std_20)
target_compile_options(kivo_playback_session PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_library(kivo_playback_pipeline STATIC
    src/core/playback/pipeline/decode_render_queue_producer.cpp
)
target_link_libraries(kivo_playback_pipeline
    PUBLIC kivo_decode_boundary kivo_render_queue)
target_include_directories(kivo_playback_pipeline PUBLIC include/)
target_compile_features(kivo_playback_pipeline PUBLIC cxx_std_20)
target_compile_options(kivo_playback_pipeline PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_library(kivo_playback_runtime STATIC
    src/core/playback/runtime/coordinator/playback_runtime_coordinator.cpp
    src/core/playback/runtime/command/playback_runtime_command_dispatch.cpp
    src/core/playback/runtime/command/playback_runtime_command_validation.cpp
    src/core/playback/runtime/flush/playback_runtime_flush.cpp
    src/core/playback/runtime/lifecycle/playback_runtime_lifecycle.cpp
    src/core/playback/runtime/open/playback_runtime_open.cpp
    src/core/playback/runtime/resource/playback_runtime_resource.cpp
    src/core/playback/runtime/seek/playback_runtime_seek.cpp
    src/core/playback/runtime/snapshot/playback_runtime_snapshot.cpp
    src/core/playback/runtime/state/playback_runtime_coordinator_state.cpp
    src/core/playback/runtime/step/playback_runtime_step.cpp
    src/core/playback/runtime/transport/playback_runtime_transport.cpp
)
target_link_libraries(kivo_playback_runtime
    PUBLIC
        kivo_playback_pipeline
        kivo_playback_session
)
target_include_directories(kivo_playback_runtime PUBLIC include/)
target_compile_features(kivo_playback_runtime PUBLIC cxx_std_20)
target_compile_options(kivo_playback_runtime PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

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

# Platform-neutral render boundary tests
add_executable(kivo_render_boundary_tests
    tests/render_boundary/render_boundary_tests_main.cpp
    tests/render_boundary/identity_capability_tests.cpp
    tests/render_boundary/format_lifecycle_tests.cpp
    tests/render_boundary/audio_block_view_tests.cpp
    tests/render_boundary/render_write_result_tests.cpp
    tests/render_boundary/boundary_interface_tests.cpp
)
target_link_libraries(kivo_render_boundary_tests PRIVATE kivo_core_render)
target_compile_features(kivo_render_boundary_tests PRIVATE cxx_std_20)
target_compile_options(kivo_render_boundary_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable(kivo_render_queue_tests
    tests/render_queue/render_queue_tests_main.cpp
    tests/render_queue/spsc_audio_block_queue_tests.cpp
    tests/render_queue/render_pump_tests.cpp
)
target_link_libraries(kivo_render_queue_tests PRIVATE kivo_render_queue)
target_compile_features(kivo_render_queue_tests PRIVATE cxx_std_20)
target_compile_options(kivo_render_queue_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable(kivo_decode_boundary_tests
    tests/decode_boundary/decode_boundary_tests_main.cpp
    tests/decode_boundary/decode_contract_tests.cpp
    tests/decode_boundary/local_file_byte_source_tests.cpp
)
target_link_libraries(kivo_decode_boundary_tests PRIVATE kivo_decode_boundary)
target_compile_features(kivo_decode_boundary_tests PRIVATE cxx_std_20)
target_compile_options(kivo_decode_boundary_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable(kivo_playback_session_tests
    tests/playback_session/runner/playback_session_tests_main.cpp
    tests/playback_session/scenario/command_lifecycle_tests.cpp
    tests/playback_session/scenario/recovery_executor_tests.cpp
    tests/playback_session/scenario/recovery_state_tests.cpp
    tests/playback_session/scenario/seek_tests.cpp
)
target_link_libraries(kivo_playback_session_tests PRIVATE kivo_playback_session)
target_compile_features(kivo_playback_session_tests PRIVATE cxx_std_20)
target_compile_options(kivo_playback_session_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable(kivo_playback_pipeline_tests
    tests/playback_pipeline/decode_render_queue_producer_tests.cpp
)
target_link_libraries(kivo_playback_pipeline_tests PRIVATE kivo_playback_pipeline)
target_compile_features(kivo_playback_pipeline_tests PRIVATE cxx_std_20)
target_compile_options(kivo_playback_pipeline_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable(kivo_playback_runtime_tests
    tests/playback_runtime/coordinator/runner/playback_runtime_tests_main.cpp
    tests/playback_runtime/coordinator/scenario/command_isolation_tests.cpp
    tests/playback_runtime/coordinator/scenario/lifecycle_tests.cpp
    tests/playback_runtime/coordinator/scenario/open_transaction_tests.cpp
    tests/playback_runtime/coordinator/scenario/seek_generation_tests.cpp
)
target_link_libraries(kivo_playback_runtime_tests
    PRIVATE
        kivo_playback_runtime
        kivo_fake_render_backend
)
target_compile_features(kivo_playback_runtime_tests PRIVATE cxx_std_20)
target_compile_options(kivo_playback_runtime_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Deterministic fake render backend
add_library(kivo_fake_render_backend STATIC
    src/testing/time/manual_clock.cpp
    src/testing/render/fake_renderer.cpp
)
target_link_libraries(kivo_fake_render_backend PUBLIC kivo_core_render)
target_include_directories(kivo_fake_render_backend PUBLIC include/)
target_compile_features(kivo_fake_render_backend PUBLIC cxx_std_20)
target_compile_options(kivo_fake_render_backend PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable(kivo_fake_renderer_tests
    tests/fake_renderer/fake_renderer_tests_main.cpp
    tests/fake_renderer/manual_clock_tests.cpp
    tests/fake_renderer/lifecycle_tests.cpp
    tests/fake_renderer/write_and_generation_tests.cpp
    tests/fake_renderer/fault_injection_tests.cpp
    tests/fake_renderer/observation_history_tests.cpp
)
target_link_libraries(kivo_fake_renderer_tests PRIVATE kivo_fake_render_backend)
target_compile_features(kivo_fake_renderer_tests PRIVATE cxx_std_20)
target_compile_options(kivo_fake_renderer_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Windows WASAPI renderer
if(WIN32)
    add_library(kivo_wasapi_renderer STATIC
        src/platform/windows/wasapi/com_apartment.cpp
        src/platform/windows/wasapi/wasapi_error.cpp
        src/platform/windows/wasapi/wasapi_format.cpp
        src/platform/windows/wasapi/wasapi_render_worker.cpp
        src/platform/windows/wasapi/wasapi_renderer.cpp
    )
    target_link_libraries(kivo_wasapi_renderer
        PUBLIC kivo_core_render kivo_render_queue
        PRIVATE ole32 uuid avrt
    )
    target_include_directories(kivo_wasapi_renderer
        PUBLIC include/
        PRIVATE src/platform/windows/wasapi
    )
    target_compile_features(kivo_wasapi_renderer PUBLIC cxx_std_20)
    target_compile_definitions(kivo_wasapi_renderer PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WIN32_WINNT=0x0A00
    )
    target_compile_options(kivo_wasapi_renderer PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )

    add_executable(kivo_wasapi_tests
        tests/platform_windows/wasapi/wasapi_tests_main.cpp
        tests/platform_windows/wasapi/format_mapping_tests.cpp
        tests/platform_windows/wasapi/error_mapping_tests.cpp
        tests/platform_windows/wasapi/render_worker_tests.cpp
    )
    target_link_libraries(kivo_wasapi_tests PRIVATE kivo_wasapi_renderer)
    target_include_directories(kivo_wasapi_tests PRIVATE
        src/platform/windows/wasapi
    )
    target_compile_features(kivo_wasapi_tests PRIVATE cxx_std_20)
    target_compile_definitions(kivo_wasapi_tests PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WIN32_WINNT=0x0A00
    )
    target_compile_options(kivo_wasapi_tests PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )

    add_executable(kivo_wasapi_tone_smoke
        tests/hardware/wasapi/wasapi_tone_smoke.cpp
    )
    target_link_libraries(kivo_wasapi_tone_smoke PRIVATE kivo_wasapi_renderer)
    target_compile_features(kivo_wasapi_tone_smoke PRIVATE cxx_std_20)
    target_compile_options(kivo_wasapi_tone_smoke PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )

    if(TARGET kivo_ffmpeg_decode_adapter)
        add_executable(kivo_ffmpeg_wasapi_playback_smoke
            tests/hardware/decode_output/ffmpeg_wasapi_playback_smoke.cpp
        )
        target_link_libraries(kivo_ffmpeg_wasapi_playback_smoke
            PRIVATE
                kivo_ffmpeg_decode_adapter
                kivo_playback_pipeline
                kivo_wasapi_renderer
        )
        target_compile_features(
            kivo_ffmpeg_wasapi_playback_smoke PRIVATE cxx_std_20)
        target_compile_options(kivo_ffmpeg_wasapi_playback_smoke PRIVATE
            $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
            $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
        )
        add_dependencies(
            kivo_ffmpeg_wasapi_playback_smoke
            kivo_ffmpeg_test_fixtures
        )
        foreach(ffmpeg_dll
            avcodec-61.dll
            avformat-61.dll
            avutil-59.dll
            swresample-5.dll)
            add_custom_command(
                TARGET kivo_ffmpeg_wasapi_playback_smoke POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${KIVO_FFMPEG_BINARY_DIR}/${ffmpeg_dll}"
                    "$<TARGET_FILE_DIR:kivo_ffmpeg_wasapi_playback_smoke>"
                VERBATIM
            )
        endforeach()
    endif()
endif()

# Public header compile checks
file(GLOB_RECURSE KIVO_PUBLIC_HEADERS
    CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/kivo/*.hpp"
)

set(KIVO_HEADER_CHECK_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/header_checks")
file(MAKE_DIRECTORY "${KIVO_HEADER_CHECK_DIR}")
set(KIVO_HEADER_CHECK_SOURCES)
set(KIVO_ALL_HEADERS_SOURCE "${KIVO_HEADER_CHECK_DIR}/all_public_headers.cpp")
file(WRITE "${KIVO_ALL_HEADERS_SOURCE}" "")

foreach(header IN LISTS KIVO_PUBLIC_HEADERS)
    string(REPLACE "/" "_" header_source_name "${header}")
    string(REPLACE "." "_" header_source_name "${header_source_name}")
    set(header_source "${KIVO_HEADER_CHECK_DIR}/${header_source_name}.cpp")
    file(WRITE "${header_source}" "#include \"${header}\"\n")
    file(APPEND "${KIVO_ALL_HEADERS_SOURCE}" "#include \"${header}\"\n")
    list(APPEND KIVO_HEADER_CHECK_SOURCES "${header_source}")
endforeach()

list(APPEND KIVO_HEADER_CHECK_SOURCES "${KIVO_ALL_HEADERS_SOURCE}")

add_library(kivo_public_header_checks OBJECT ${KIVO_HEADER_CHECK_SOURCES})
target_link_libraries(kivo_public_header_checks PRIVATE kivo_core_render)
target_compile_features(kivo_public_header_checks PRIVATE cxx_std_20)
target_compile_options(kivo_public_header_checks PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_dependencies(kivo_contract_tests kivo_public_header_checks)
add_dependencies(kivo_capability_tests kivo_public_header_checks)
add_dependencies(kivo_render_boundary_tests kivo_public_header_checks)
add_dependencies(kivo_render_queue_tests kivo_public_header_checks)
add_dependencies(kivo_decode_boundary_tests kivo_public_header_checks)
add_dependencies(kivo_playback_session_tests kivo_public_header_checks)
add_dependencies(kivo_playback_pipeline_tests kivo_public_header_checks)
add_dependencies(kivo_playback_runtime_tests kivo_public_header_checks)
add_dependencies(kivo_fake_renderer_tests kivo_public_header_checks)
if(TARGET kivo_wasapi_tests)
    add_dependencies(kivo_wasapi_tests kivo_public_header_checks)
    add_dependencies(kivo_wasapi_tone_smoke kivo_public_header_checks)
endif()
if(TARGET kivo_ffmpeg_decode_tests)
    add_dependencies(kivo_ffmpeg_decode_tests kivo_public_header_checks)
endif()
if(TARGET kivo_ffmpeg_wasapi_playback_smoke)
    add_dependencies(
        kivo_ffmpeg_wasapi_playback_smoke
        kivo_public_header_checks)
endif()

# Register with CTest
enable_testing()
add_test(NAME contract_tests COMMAND kivo_contract_tests)
add_test(NAME capability_tests COMMAND kivo_capability_tests)
add_test(NAME render_boundary_tests COMMAND kivo_render_boundary_tests)
add_test(NAME render_queue_tests COMMAND kivo_render_queue_tests)
add_test(NAME decode_boundary_tests COMMAND kivo_decode_boundary_tests)
add_test(NAME playback_session_tests COMMAND kivo_playback_session_tests)
add_test(NAME playback_pipeline_tests COMMAND kivo_playback_pipeline_tests)
add_test(NAME playback_runtime_tests COMMAND kivo_playback_runtime_tests)
add_test(NAME fake_renderer_tests COMMAND kivo_fake_renderer_tests)
if(TARGET kivo_wasapi_tests)
    add_test(NAME wasapi_tests COMMAND kivo_wasapi_tests)
endif()
if(TARGET kivo_ffmpeg_decode_tests)
    add_test(
        NAME ffmpeg_decode_tests
        COMMAND kivo_ffmpeg_decode_tests "${KIVO_FFMPEG_FIXTURE_DIR}")
endif()
