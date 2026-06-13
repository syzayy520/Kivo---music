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
    VERSION 1.0.0
    LANGUAGES C CXX
    DESCRIPTION "Kivo Audio Core — C++ Windows Desktop Audio Engine"
)

# --- C ABI Standard Policy ---
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# --- C++ Standard Policy ---
# C++20 is required. No extensions. No fallback.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(
        $<$<CONFIG:Release>:/Brepro>
        $<$<CONFIG:Release>:/ZH:SHA_256>
    )
    add_link_options(
        $<$<CONFIG:Release>:/Brepro>
    )
endif()

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

add_library(kivo_output_truth STATIC
    src/core/output/truth/evaluation/bit_perfect_truth_evaluator.cpp
    src/core/output/truth/evidence/bit_perfect_evidence_builder.cpp
)
target_link_libraries(kivo_output_truth PUBLIC kivo_core_render)
target_include_directories(kivo_output_truth PUBLIC include/)
target_compile_features(kivo_output_truth PUBLIC cxx_std_20)
target_compile_options(kivo_output_truth PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
target_link_libraries(kivo_core_render INTERFACE kivo_core_contract)

add_library(kivo_audio_processing STATIC
    src/core/processing/chain/audio_processing_chain.cpp
    src/core/processing/gain/audio_gain_application.cpp
    src/core/processing/planning/audio_processing_plan.cpp
)
target_link_libraries(kivo_audio_processing PUBLIC kivo_core_contract)
target_include_directories(kivo_audio_processing
    PUBLIC include/
    PRIVATE src
)
target_compile_features(kivo_audio_processing PUBLIC cxx_std_20)
target_compile_options(kivo_audio_processing PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

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
        src/adapters/ffmpeg/container/validation/known_container_extent.cpp
        src/adapters/ffmpeg/codec/ffmpeg_audio_decoder.cpp
        src/adapters/ffmpeg/conversion/converter/ffmpeg_audio_converter.cpp
        src/adapters/ffmpeg/conversion/converter/ffmpeg_audio_convert_samples.cpp
        src/adapters/ffmpeg/conversion/policy/ffmpeg_resampler_policy.cpp
        src/adapters/ffmpeg/conversion/truth/ffmpeg_conversion_delay.cpp
        src/adapters/ffmpeg/mapping/ffmpeg_audio_format.cpp
        src/adapters/ffmpeg/session/ffmpeg_audio_decode_control.cpp
        src/adapters/ffmpeg/session/ffmpeg_audio_decode_facade.cpp
        src/adapters/ffmpeg/session/ffmpeg_audio_decode_open.cpp
        src/adapters/ffmpeg/session/ffmpeg_audio_decode_step.cpp
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
        PRIVATE src
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
    set(KIVO_FFMPEG_VBR_MP3_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/tone_vbr.mp3")
    set(KIVO_FFMPEG_AAC_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/tone_aac.m4a")
    set(KIVO_FFMPEG_ALAC_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/tone_alac.m4a")
    set(KIVO_FFMPEG_VORBIS_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/tone_vorbis.ogg")
    set(KIVO_FFMPEG_OPUS_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/tone_opus.ogg")
    set(KIVO_FFMPEG_MONO_WAVE_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/mono_22050.wav")
    set(KIVO_FFMPEG_HIRES_FLAC_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/stereo_96000.flac")
    set(KIVO_FFMPEG_SURROUND_WAVE_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/surround_48000.wav")
    set(KIVO_FFMPEG_LONG_WAVE_FIXTURE
        "${KIVO_FFMPEG_FIXTURE_DIR}/long_pcm_s16le.wav")
    add_custom_command(
        OUTPUT
            "${KIVO_FFMPEG_WAVE_FIXTURE}"
            "${KIVO_FFMPEG_FLAC_FIXTURE}"
            "${KIVO_FFMPEG_MP3_FIXTURE}"
            "${KIVO_FFMPEG_VBR_MP3_FIXTURE}"
            "${KIVO_FFMPEG_AAC_FIXTURE}"
            "${KIVO_FFMPEG_ALAC_FIXTURE}"
            "${KIVO_FFMPEG_VORBIS_FIXTURE}"
            "${KIVO_FFMPEG_OPUS_FIXTURE}"
            "${KIVO_FFMPEG_MONO_WAVE_FIXTURE}"
            "${KIVO_FFMPEG_HIRES_FLAC_FIXTURE}"
            "${KIVO_FFMPEG_SURROUND_WAVE_FIXTURE}"
            "${KIVO_FFMPEG_LONG_WAVE_FIXTURE}"
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
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=880:duration=2:sample_rate=44100"
            -ac 2 -c:a libmp3lame -q:a 2 "${KIVO_FFMPEG_VBR_MP3_FIXTURE}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=987.77:duration=2:sample_rate=44100"
            -ac 2 -c:a aac -b:a 160k -movflags +faststart
            "${KIVO_FFMPEG_AAC_FIXTURE}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=1046.50:duration=2:sample_rate=44100"
            -ac 2 -c:a alac -movflags +faststart
            "${KIVO_FFMPEG_ALAC_FIXTURE}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=1174.66:duration=2:sample_rate=44100"
            -ac 2 -c:a libvorbis -q:a 5
            -fflags +bitexact -flags:a +bitexact -serial_offset 0
            "${KIVO_FFMPEG_VORBIS_FIXTURE}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=1318.51:duration=2:sample_rate=48000"
            -ac 2 -c:a libopus -b:a 128k
            -fflags +bitexact -flags:a +bitexact -serial_offset 0
            "${KIVO_FFMPEG_OPUS_FIXTURE}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=261.63:duration=2:sample_rate=22050"
            -ac 1 -c:a pcm_s16le "${KIVO_FFMPEG_MONO_WAVE_FIXTURE}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=329.63:duration=2:sample_rate=96000"
            -ac 2 -c:a flac "${KIVO_FFMPEG_HIRES_FLAC_FIXTURE}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=392:duration=2:sample_rate=48000"
            -ac 6 -channel_layout 5.1 -c:a pcm_s16le
            "${KIVO_FFMPEG_SURROUND_WAVE_FIXTURE}"
        COMMAND "${KIVO_FFMPEG_BINARY_DIR}/ffmpeg.exe"
            -nostdin -hide_banner -loglevel error -y
            -f lavfi -i "sine=frequency=440:duration=30:sample_rate=44100"
            -ac 2 -c:a pcm_s16le "${KIVO_FFMPEG_LONG_WAVE_FIXTURE}"
        VERBATIM
    )
    add_custom_target(kivo_ffmpeg_test_fixtures
        DEPENDS
            "${KIVO_FFMPEG_WAVE_FIXTURE}"
            "${KIVO_FFMPEG_FLAC_FIXTURE}"
            "${KIVO_FFMPEG_MP3_FIXTURE}"
            "${KIVO_FFMPEG_VBR_MP3_FIXTURE}"
            "${KIVO_FFMPEG_AAC_FIXTURE}"
            "${KIVO_FFMPEG_ALAC_FIXTURE}"
            "${KIVO_FFMPEG_VORBIS_FIXTURE}"
            "${KIVO_FFMPEG_OPUS_FIXTURE}"
            "${KIVO_FFMPEG_MONO_WAVE_FIXTURE}"
            "${KIVO_FFMPEG_HIRES_FLAC_FIXTURE}"
            "${KIVO_FFMPEG_SURROUND_WAVE_FIXTURE}"
            "${KIVO_FFMPEG_LONG_WAVE_FIXTURE}"
    )

    add_executable(kivo_ffmpeg_decode_tests
        tests/ffmpeg_decode/fixture/assertion/abnormal_media_decode.cpp
        tests/ffmpeg_decode/fixture/assertion/fixture_decode_assertion.cpp
        tests/ffmpeg_decode/fixture/mutation/media_mutation_file.cpp
        tests/ffmpeg_decode/runner/ffmpeg_decode_tests_main.cpp
        tests/ffmpeg_decode/scenario/control/seek_flush_tests.cpp
        tests/ffmpeg_decode/scenario/corruption/header_corruption_tests.cpp
        tests/ffmpeg_decode/scenario/corruption/trailing_junk_tests.cpp
        tests/ffmpeg_decode/scenario/cue/unsupported_cue_tests.cpp
        tests/ffmpeg_decode/scenario/decode/baseline_decode_tests.cpp
        tests/ffmpeg_decode/scenario/format/extended_format_matrix_tests.cpp
        tests/ffmpeg_decode/scenario/policy/conversion_policy_tests.cpp
        tests/ffmpeg_decode/scenario/policy/conversion_truth_tests.cpp
        tests/ffmpeg_decode/scenario/policy/dsd_support_policy_tests.cpp
        tests/ffmpeg_decode/scenario/truncation/truncation_matrix_tests.cpp
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

if(TARGET kivo_ffmpeg_decode_adapter)
    add_executable(kivo_audio_core_performance_probe
        tools/validation/performance/measurement/decode_latency_measurement.cpp
        tools/validation/performance/measurement/queue_timing_measurement.cpp
        tools/validation/performance/reporting/performance_evidence_writer.cpp
        tools/validation/performance/runner/audio_core_performance_probe_main.cpp
    )
    target_link_libraries(kivo_audio_core_performance_probe
        PRIVATE
            kivo_ffmpeg_decode_adapter
            kivo_render_queue
    )
    target_compile_features(
        kivo_audio_core_performance_probe PRIVATE cxx_std_20)
    target_compile_options(kivo_audio_core_performance_probe PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
    add_dependencies(
        kivo_audio_core_performance_probe
        kivo_ffmpeg_test_fixtures)

    foreach(ffmpeg_dll
        avcodec-61.dll
        avformat-61.dll
        avutil-59.dll
        swresample-5.dll)
        add_custom_command(
            TARGET kivo_audio_core_performance_probe POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${KIVO_FFMPEG_BINARY_DIR}/${ffmpeg_dll}"
                "$<TARGET_FILE_DIR:kivo_audio_core_performance_probe>"
            VERBATIM
        )
    endforeach()
endif()

# Serialized product-control session state
add_library(kivo_playback_session STATIC
    src/core/playback/recovery/executor/playback_recovery_executor.cpp
    src/core/playback/session/command/playback_session_command.cpp
    src/core/playback/session/controller/playback_session_controller.cpp
    src/core/playback/session/drain/playback_session_drain.cpp
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
    src/core/playback/pipeline/producer/decode_render_queue_producer.cpp
    src/core/playback/pipeline/step/decode_render_queue_producer_step.cpp
    src/core/playback/pipeline/transfer/decode_render_queue_producer_transfer.cpp
)
target_link_libraries(kivo_playback_pipeline
    PUBLIC kivo_decode_boundary kivo_render_queue kivo_audio_processing)
target_include_directories(kivo_playback_pipeline PUBLIC include/)
target_compile_features(kivo_playback_pipeline PUBLIC cxx_std_20)
target_compile_options(kivo_playback_pipeline PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_library(kivo_playback_gapless STATIC
    src/core/playback/gapless/coordinator/gapless_transition_coordinator_facade.cpp
    src/core/playback/gapless/coordinator/gapless_transition_coordinator_prepare.cpp
    src/core/playback/gapless/coordinator/gapless_transition_coordinator_step.cpp
    src/core/playback/gapless/position/gapless_position_estimator.cpp
    src/core/playback/gapless/segment/whole_file_segment_plan.cpp
    src/core/playback/gapless/transition/gapless_transition_evaluator.cpp
)
target_link_libraries(kivo_playback_gapless PUBLIC kivo_playback_pipeline)
target_include_directories(kivo_playback_gapless PUBLIC include/)
target_compile_features(kivo_playback_gapless PUBLIC cxx_std_20)
target_compile_options(kivo_playback_gapless PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_library(kivo_playback_runtime STATIC
    src/core/playback/runtime/coordinator/playback_runtime_coordinator.cpp
    src/core/playback/runtime/command/playback_runtime_command_dispatch.cpp
    src/core/playback/runtime/command/playback_runtime_command_validation.cpp
    src/core/playback/runtime/drain/playback_runtime_drain.cpp
    src/core/playback/runtime/flush/playback_runtime_flush.cpp
    src/core/playback/runtime/lifecycle/playback_runtime_lifecycle.cpp
    src/core/playback/runtime/open/playback_runtime_open.cpp
    src/core/playback/runtime/recovery/playback_runtime_decode_failure.cpp
    src/core/playback/runtime/recovery/playback_runtime_device_recovery.cpp
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
    tests/contracts/format/dsd/dsd_rate_tests.cpp
    tests/contracts/format/dsd/native_dsd_format_tests.cpp
    tests/contracts/format/dsd/dop_marker_tests.cpp
    tests/contracts/format/dsd/dop_carrier_format_tests.cpp
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

add_executable(kivo_output_truth_tests
    tests/output_truth/runner/output_truth_tests_main.cpp
    tests/output_truth/scenario/evidence/evidence_builder_tests.cpp
    tests/output_truth/scenario/evaluation/format_evaluation_tests.cpp
    tests/output_truth/scenario/evaluation/mode_evaluation_tests.cpp
    tests/output_truth/scenario/evaluation/mutation_evaluation_tests.cpp
)

add_executable(kivo_audio_processing_tests
    tests/audio_processing/runner/audio_processing_tests_main.cpp
    tests/audio_processing/scenario/bypass/bypass_tests.cpp
    tests/audio_processing/scenario/formats/sample_format_tests.cpp
    tests/audio_processing/scenario/gain/gain_plan_tests.cpp
    tests/audio_processing/scenario/policy/policy_tests.cpp
)

add_executable(kivo_hires_pcm_tests
    tests/hires_pcm/runner/hires_pcm_tests_main.cpp
    tests/hires_pcm/scenario/boundary/non_pcm_boundary_tests.cpp
    tests/hires_pcm/scenario/descriptor/hires_pcm_descriptor_tests.cpp
)
target_link_libraries(kivo_audio_processing_tests
    PRIVATE kivo_audio_processing)
target_compile_features(kivo_audio_processing_tests PRIVATE cxx_std_20)
target_compile_options(kivo_audio_processing_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
target_link_libraries(kivo_output_truth_tests PRIVATE kivo_output_truth)
target_compile_features(kivo_output_truth_tests PRIVATE cxx_std_20)
target_compile_options(kivo_output_truth_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
target_link_libraries(kivo_hires_pcm_tests PRIVATE kivo_core_contract)
target_compile_features(kivo_hires_pcm_tests PRIVATE cxx_std_20)
target_compile_options(kivo_hires_pcm_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
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
    tests/decode_boundary/runner/decode_boundary_tests_main.cpp
    tests/decode_boundary/scenario/contract/block/decoded_block_contract_tests.cpp
    tests/decode_boundary/scenario/contract/control/control_result_contract_tests.cpp
    tests/decode_boundary/scenario/contract/media_probe/media_probe_contract_tests.cpp
    tests/decode_boundary/scenario/contract/open_result/open_result_contract_tests.cpp
    tests/decode_boundary/scenario/contract/step/step_result_contract_tests.cpp
    tests/decode_boundary/scenario/source/large_file/size_seek/large_file_byte_source_tests.cpp
    tests/decode_boundary/scenario/source/local_file/failure/local_file_failure_tests.cpp
    tests/decode_boundary/scenario/source/local_file/open/local_file_open_tests.cpp
    tests/decode_boundary/scenario/source/local_file/read/local_file_read_tests.cpp
    tests/decode_boundary/scenario/source/local_file/seek/local_file_seek_tests.cpp
    tests/decode_boundary/scenario/media/media_support_tests.cpp
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
    tests/playback_session/scenario/decode_failure_classification_tests.cpp
    tests/playback_session/scenario/drain_lifecycle_tests.cpp
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
    tests/playback_pipeline/runner/playback_pipeline_tests_main.cpp
    tests/playback_pipeline/scenario/backpressure/backpressure_tests.cpp
    tests/playback_pipeline/scenario/eos/eos_chunking_tests.cpp
    tests/playback_pipeline/scenario/generation/generation_tests.cpp
    tests/playback_pipeline/scenario/processing/processing_tests.cpp
    tests/playback_pipeline/scenario/validation/validation_tests.cpp
)
target_link_libraries(kivo_playback_pipeline_tests PRIVATE kivo_playback_pipeline)
target_compile_features(kivo_playback_pipeline_tests PRIVATE cxx_std_20)
target_compile_options(kivo_playback_pipeline_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable(kivo_playback_gapless_tests
    tests/playback_gapless/runner/playback_gapless_tests_main.cpp
    tests/playback_gapless/scenario/compatibility/transition_compatibility_tests.cpp
    tests/playback_gapless/scenario/continuity/queue_continuity_tests.cpp
    tests/playback_gapless/scenario/continuity/transition_coordinator_tests.cpp
    tests/playback_gapless/scenario/position/position_estimator_tests.cpp
    tests/playback_gapless/scenario/segment/whole_file_segment_tests.cpp
)
target_link_libraries(kivo_playback_gapless_tests PRIVATE kivo_playback_gapless)
target_compile_features(kivo_playback_gapless_tests PRIVATE cxx_std_20)
target_compile_options(kivo_playback_gapless_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable(kivo_playback_runtime_tests
    tests/playback_runtime/coordinator/runner/playback_runtime_tests_main.cpp
    tests/playback_runtime/coordinator/scenario/active_operation_shutdown_tests.cpp
    tests/playback_runtime/coordinator/scenario/command_isolation_tests.cpp
    tests/playback_runtime/coordinator/scenario/decode_failure_recovery_tests.cpp
    tests/playback_runtime/coordinator/scenario/eos_drain_tests.cpp
    tests/playback_runtime/coordinator/scenario/flush_recovery_tests.cpp
    tests/playback_runtime/coordinator/scenario/device_recovery_tests.cpp
    tests/playback_runtime/coordinator/scenario/lifecycle_tests.cpp
    tests/playback_runtime/coordinator/scenario/open_transaction_tests.cpp
    tests/playback_runtime/coordinator/scenario/seek_generation_tests.cpp
    tests/playback_runtime/coordinator/scenario/track_switch_race_tests.cpp
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

add_executable(kivo_observability_tests
    tests/observability/runner/observability_tests_main.cpp
    tests/observability/scenario/category/decode_failure_category_tests.cpp
    tests/observability/scenario/snapshot/audio_diagnostic_snapshot_tests.cpp
)
target_include_directories(kivo_observability_tests PRIVATE include/)
target_compile_features(kivo_observability_tests PRIVATE cxx_std_20)
target_compile_options(kivo_observability_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

# Deterministic fake render backend
add_library(kivo_fake_render_backend STATIC
    src/testing/time/manual_clock.cpp
    src/testing/render/consumption/fake_renderer_consume.cpp
    src/testing/render/fault/fake_renderer_fault.cpp
    src/testing/render/lifecycle/fake_renderer_control.cpp
    src/testing/render/lifecycle/fake_renderer_open.cpp
    src/testing/render/observation/fake_renderer_observation.cpp
    src/testing/render/write/fake_renderer_write.cpp
)
target_link_libraries(kivo_fake_render_backend PUBLIC kivo_core_render)
target_include_directories(kivo_fake_render_backend PUBLIC include/)
target_compile_features(kivo_fake_render_backend PUBLIC cxx_std_20)
target_compile_options(kivo_fake_render_backend PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_executable(kivo_fake_renderer_tests
    tests/fake_renderer/runner/fake_renderer_tests_main.cpp
    tests/fake_renderer/scenario/manual_clock_tests.cpp
    tests/fake_renderer/scenario/lifecycle_tests.cpp
    tests/fake_renderer/scenario/write_and_generation_tests.cpp
    tests/fake_renderer/scenario/fault_injection_tests.cpp
    tests/fake_renderer/scenario/device_lifecycle_fault_tests.cpp
    tests/fake_renderer/scenario/observation_history_tests.cpp
)
target_link_libraries(kivo_fake_renderer_tests PRIVATE kivo_fake_render_backend)
target_compile_features(kivo_fake_renderer_tests PRIVATE cxx_std_20)
target_compile_options(kivo_fake_renderer_tests PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

if(WIN32 AND TARGET kivo_ffmpeg_decode_adapter)
    add_executable(kivo_stability_tests
        tests/stability/runner/stability_tests_main.cpp
        tests/stability/scenario/repeated_fake_renderer_lifecycle_tests.cpp
        tests/stability/scenario/repeated_ffmpeg_resource_tests.cpp
    )
    target_link_libraries(kivo_stability_tests
        PRIVATE
            kivo_fake_render_backend
            kivo_ffmpeg_decode_adapter
            psapi
    )
    target_compile_features(kivo_stability_tests PRIVATE cxx_std_20)
    target_compile_definitions(kivo_stability_tests PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    target_compile_options(kivo_stability_tests PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc /wd4127>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
    add_dependencies(kivo_stability_tests kivo_ffmpeg_test_fixtures)
    foreach(ffmpeg_dll
        avcodec-61.dll
        avformat-61.dll
        avutil-59.dll
        swresample-5.dll)
        add_custom_command(TARGET kivo_stability_tests POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${KIVO_FFMPEG_BINARY_DIR}/${ffmpeg_dll}"
                "$<TARGET_FILE_DIR:kivo_stability_tests>"
            VERBATIM
        )
    endforeach()

endif()

# Windows WASAPI renderer
if(WIN32)
    add_library(kivo_wasapi_renderer STATIC
        src/platform/windows/wasapi/apartment/com_apartment.cpp
        src/platform/windows/wasapi/device/notification/default_render_endpoint_observer.cpp
        src/platform/windows/wasapi/error/wasapi_error.cpp
        src/platform/windows/wasapi/format/wasapi_format.cpp
        src/platform/windows/wasapi/power/notification/audio_power_transition_observer.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_control.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_drain.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_facade.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_failure.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_notification.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_open.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_open_complete.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_open_environment.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_open_service.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_query.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_exclusive_open.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_shared_open.cpp
        src/platform/windows/wasapi/renderer/wasapi_renderer_write.cpp
        src/platform/windows/wasapi/worker/wasapi_render_worker_facade.cpp
        src/platform/windows/wasapi/worker/wasapi_render_worker_lifecycle.cpp
        src/platform/windows/wasapi/worker/wasapi_render_worker_run.cpp
    )
    target_link_libraries(kivo_wasapi_renderer
        PUBLIC kivo_core_render kivo_render_queue
        PRIVATE ole32 uuid avrt powrprof
    )
    target_include_directories(kivo_wasapi_renderer
        PUBLIC include/
        PRIVATE src
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
        tests/platform_windows/wasapi/runner/wasapi_tests_main.cpp
        tests/platform_windows/wasapi/scenario/format_mapping_tests.cpp
        tests/platform_windows/wasapi/scenario/error_mapping_tests.cpp
        tests/platform_windows/wasapi/scenario/endpoint_notification_tests.cpp
        tests/platform_windows/wasapi/scenario/power_notification_tests.cpp
        tests/platform_windows/wasapi/scenario/render_worker_tests.cpp
    )
    target_link_libraries(kivo_wasapi_tests PRIVATE kivo_wasapi_renderer)
    target_include_directories(kivo_wasapi_tests PRIVATE
        src
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
        tests/hardware/wasapi/fixture/tone_generator.cpp
        tests/hardware/wasapi/fixture/tone_options.cpp
        tests/hardware/wasapi/runner/wasapi_tone_smoke_main.cpp
        tests/hardware/wasapi/scenario/wasapi_tone_cycle.cpp
        tests/hardware/wasapi/scenario/wasapi_tone_playback.cpp
    )
    target_link_libraries(kivo_wasapi_tone_smoke PRIVATE kivo_wasapi_renderer)
    target_compile_features(kivo_wasapi_tone_smoke PRIVATE cxx_std_20)
    target_compile_options(kivo_wasapi_tone_smoke PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )

    add_executable(kivo_wasapi_device_matrix
        tests/hardware/device_matrix/platform/windows/apartment/com_apartment.cpp
        tests/hardware/device_matrix/platform/windows/driver/pnp_device_probe.cpp
        tests/hardware/device_matrix/platform/windows/endpoint/endpoint_capability_probe.cpp
        tests/hardware/device_matrix/platform/windows/endpoint/endpoint_inventory.cpp
        tests/hardware/device_matrix/platform/windows/endpoint/endpoint_metadata_probe.cpp
        tests/hardware/device_matrix/runner/wasapi_device_matrix_main.cpp
        tests/hardware/device_matrix/scenario/device_matrix_report.cpp
        tests/hardware/device_matrix/scenario/device_matrix_validation.cpp
        tests/hardware/device_matrix/serialization/device_matrix_json_report.cpp
    )
    target_link_libraries(kivo_wasapi_device_matrix
        PRIVATE ole32 setupapi)
    target_compile_features(kivo_wasapi_device_matrix PRIVATE cxx_std_20)
    target_compile_options(kivo_wasapi_device_matrix PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )

    if(TARGET kivo_ffmpeg_decode_adapter)
        add_executable(kivo_ffmpeg_wasapi_playback_smoke
            tests/hardware/decode_output/runner/ffmpeg_wasapi_playback_smoke_main.cpp
            tests/hardware/decode_output/scenario/decode_output_smoke_complete.cpp
            tests/hardware/decode_output/scenario/decode_output_smoke_open.cpp
            tests/hardware/decode_output/scenario/decode_output_smoke_play.cpp
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

if(WIN32
    AND TARGET kivo_ffmpeg_decode_adapter
    AND TARGET kivo_wasapi_renderer)
    enable_language(RC)
    set(KIVO_HOST_VERSION_RESOURCE
        "${CMAKE_CURRENT_BINARY_DIR}/generated/resource/kivo_audio_core_version.rc")
    configure_file(
        src/host/abi/resource/kivo_audio_core_version.rc.in
        "${KIVO_HOST_VERSION_RESOURCE}"
        @ONLY
    )
    add_library(kivo_host_abi SHARED
        "${KIVO_HOST_VERSION_RESOURCE}"
        src/host/abi/export/kivo_audio_core.def
        src/host/abi/api/host_audio_api.cpp
        src/host/abi/diagnostic/host_diagnostic_snapshot.cpp
        src/host/abi/engine/host_audio_engine_command.cpp
        src/host/abi/engine/host_audio_engine_configuration.cpp
        src/host/abi/engine/host_audio_engine_lifecycle.cpp
        src/host/abi/engine/host_audio_engine_pump.cpp
        src/host/abi/engine/host_audio_engine_snapshot.cpp
        src/host/abi/handle/host_handle_registry.cpp
        src/host/abi/mapping/host_result_mapping.cpp
        src/host/abi/source/host_callback_byte_source.cpp
        src/host/abi/validation/host_abi_validation.cpp
    )
    target_link_libraries(kivo_host_abi
        PRIVATE
            kivo_ffmpeg_decode_adapter
            kivo_playback_runtime
            kivo_wasapi_renderer
    )
    target_include_directories(kivo_host_abi
        PUBLIC include/
        PRIVATE src
    )
    target_compile_features(kivo_host_abi PRIVATE cxx_std_20)
    target_compile_definitions(kivo_host_abi PRIVATE
        KIVO_AUDIO_HOST_ABI_BUILD
        WIN32_LEAN_AND_MEAN
        NOMINMAX
        _WIN32_WINNT=0x0A00
    )
    target_compile_options(kivo_host_abi PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
        $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
        $<$<CXX_COMPILER_ID:MSVC>:/guard:cf>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/Zi>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wextra>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wpedantic>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Werror>
    )
    target_link_options(kivo_host_abi PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/guard:cf>
        $<$<CXX_COMPILER_ID:MSVC>:/DYNAMICBASE>
        $<$<CXX_COMPILER_ID:MSVC>:/NXCOMPAT>
        $<$<CXX_COMPILER_ID:MSVC>:/CETCOMPAT>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/DEBUG:FULL>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/OPT:REF>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/OPT:ICF>
        $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/PDBALTPATH:kivo_audio_core.pdb>
    )
    set_target_properties(kivo_host_abi PROPERTIES
        OUTPUT_NAME "kivo_audio_core"
        PDB_NAME "kivo_audio_core"
        PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        MSVC_RUNTIME_LIBRARY
            "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )

    add_executable(kivo_host_abi_tests
        tests/host_abi/compatibility/c_header_compile.c
        tests/host_abi/runner/host_abi_tests_main.cpp
        tests/host_abi/scenario/compatibility_tests.cpp
        tests/host_abi/scenario/handle_lifecycle_tests.cpp
        tests/host_abi/scenario/source_ownership_tests.cpp
    )
    target_link_libraries(kivo_host_abi_tests PRIVATE kivo_host_abi)
    target_compile_features(kivo_host_abi_tests PRIVATE cxx_std_20)
    target_compile_options(kivo_host_abi_tests PRIVATE
        $<$<COMPILE_LANG_AND_ID:C,MSVC>:/W4 /WX /utf-8>
        $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/W4 /WX /utf-8 /EHsc>
        $<$<AND:$<COMPILE_LANGUAGE:C>,$<NOT:$<C_COMPILER_ID:MSVC>>>:-Wall -Wextra -Wpedantic -Werror>
        $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-Wall -Wextra -Wpedantic -Werror>
    )
    foreach(ffmpeg_dll
        avcodec-61.dll
        avformat-61.dll
        avutil-59.dll
        swresample-5.dll)
        add_custom_command(TARGET kivo_host_abi_tests POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${KIVO_FFMPEG_BINARY_DIR}/${ffmpeg_dll}"
                "$<TARGET_FILE_DIR:kivo_host_abi_tests>"
            VERBATIM
        )
    endforeach()

    add_executable(kivo_host_abi_playback_smoke
        tests/hardware/host_abi/fixture/local_callback_source.cpp
        tests/hardware/host_abi/runner/host_abi_playback_smoke_main.cpp
        tests/hardware/host_abi/scenario/host_abi_playback_cycle.cpp
    )
    target_link_libraries(
        kivo_host_abi_playback_smoke
        PRIVATE kivo_host_abi)
    target_compile_features(
        kivo_host_abi_playback_smoke PRIVATE cxx_std_20)
    target_compile_options(kivo_host_abi_playback_smoke PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8 /EHsc>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
    )
    foreach(ffmpeg_dll
        avcodec-61.dll
        avformat-61.dll
        avutil-59.dll
        swresample-5.dll)
        add_custom_command(TARGET kivo_host_abi_playback_smoke POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${KIVO_FFMPEG_BINARY_DIR}/${ffmpeg_dll}"
                "$<TARGET_FILE_DIR:kivo_host_abi_playback_smoke>"
            VERBATIM
        )
    endforeach()
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

file(GLOB_RECURSE KIVO_PUBLIC_C_HEADERS
    CONFIGURE_DEPENDS
    RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/kivo/*.h"
)
set(KIVO_C_HEADER_CHECK_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated/c_header_checks")
file(MAKE_DIRECTORY "${KIVO_C_HEADER_CHECK_DIR}")
set(KIVO_C_HEADER_CHECK_SOURCES)
foreach(header IN LISTS KIVO_PUBLIC_C_HEADERS)
    string(REPLACE "/" "_" header_source_name "${header}")
    string(REPLACE "." "_" header_source_name "${header_source_name}")
    set(header_source
        "${KIVO_C_HEADER_CHECK_DIR}/${header_source_name}.c")
    file(WRITE "${header_source}" "#include \"${header}\"\n")
    list(APPEND KIVO_C_HEADER_CHECK_SOURCES "${header_source}")
endforeach()
add_library(kivo_public_c_header_checks
    OBJECT
    ${KIVO_C_HEADER_CHECK_SOURCES}
)
target_include_directories(kivo_public_c_header_checks PRIVATE include/)
target_compile_features(kivo_public_c_header_checks PRIVATE c_std_11)
target_compile_options(kivo_public_c_header_checks PRIVATE
    $<$<C_COMPILER_ID:MSVC>:/W4 /WX /utf-8>
    $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_dependencies(kivo_contract_tests kivo_public_header_checks)
add_dependencies(kivo_capability_tests kivo_public_header_checks)
add_dependencies(kivo_render_boundary_tests kivo_public_header_checks)
add_dependencies(kivo_output_truth_tests kivo_public_header_checks)
add_dependencies(kivo_audio_processing_tests kivo_public_header_checks)
add_dependencies(kivo_hires_pcm_tests kivo_public_header_checks)
add_dependencies(kivo_render_queue_tests kivo_public_header_checks)
add_dependencies(kivo_decode_boundary_tests kivo_public_header_checks)
add_dependencies(kivo_playback_session_tests kivo_public_header_checks)
add_dependencies(kivo_playback_pipeline_tests kivo_public_header_checks)
add_dependencies(kivo_playback_gapless_tests kivo_public_header_checks)
add_dependencies(kivo_playback_runtime_tests kivo_public_header_checks)
add_dependencies(kivo_observability_tests kivo_public_header_checks)
add_dependencies(kivo_fake_renderer_tests kivo_public_header_checks)
if(TARGET kivo_stability_tests)
    add_dependencies(kivo_stability_tests kivo_public_header_checks)
endif()
if(TARGET kivo_wasapi_tests)
    add_dependencies(kivo_wasapi_tests kivo_public_header_checks)
    add_dependencies(kivo_wasapi_tone_smoke kivo_public_header_checks)
    add_dependencies(kivo_wasapi_device_matrix kivo_public_header_checks)
endif()
if(TARGET kivo_ffmpeg_decode_tests)
    add_dependencies(kivo_ffmpeg_decode_tests kivo_public_header_checks)
endif()
if(TARGET kivo_ffmpeg_wasapi_playback_smoke)
    add_dependencies(
        kivo_ffmpeg_wasapi_playback_smoke
        kivo_public_header_checks)
endif()
if(TARGET kivo_host_abi_tests)
    add_dependencies(
        kivo_host_abi_tests
        kivo_public_header_checks
        kivo_public_c_header_checks)
endif()
if(TARGET kivo_host_abi_playback_smoke)
    add_dependencies(
        kivo_host_abi_playback_smoke
        kivo_public_header_checks
        kivo_public_c_header_checks)
endif()

# Register with CTest
enable_testing()
add_test(NAME contract_tests COMMAND kivo_contract_tests)
add_test(NAME capability_tests COMMAND kivo_capability_tests)
add_test(NAME render_boundary_tests COMMAND kivo_render_boundary_tests)
add_test(NAME output_truth_tests COMMAND kivo_output_truth_tests)
add_test(NAME audio_processing_tests COMMAND kivo_audio_processing_tests)
add_test(NAME hires_pcm_tests COMMAND kivo_hires_pcm_tests)
add_test(NAME render_queue_tests COMMAND kivo_render_queue_tests)
add_test(NAME decode_boundary_tests COMMAND kivo_decode_boundary_tests)
add_test(NAME playback_session_tests COMMAND kivo_playback_session_tests)
add_test(NAME playback_pipeline_tests COMMAND kivo_playback_pipeline_tests)
add_test(NAME playback_gapless_tests COMMAND kivo_playback_gapless_tests)
add_test(NAME playback_runtime_tests COMMAND kivo_playback_runtime_tests)
add_test(NAME observability_tests COMMAND kivo_observability_tests)
add_test(NAME fake_renderer_tests COMMAND kivo_fake_renderer_tests)
if(TARGET kivo_stability_tests)
    add_test(
        NAME stability_tests
        COMMAND kivo_stability_tests "${KIVO_FFMPEG_FIXTURE_DIR}")
endif()
if(TARGET kivo_wasapi_tests)
    add_test(NAME wasapi_tests COMMAND kivo_wasapi_tests)
endif()
if(TARGET kivo_ffmpeg_decode_tests)
    add_test(
        NAME ffmpeg_decode_tests
        COMMAND kivo_ffmpeg_decode_tests "${KIVO_FFMPEG_FIXTURE_DIR}")
endif()
if(TARGET kivo_host_abi_tests)
    add_test(NAME host_abi_tests COMMAND kivo_host_abi_tests)
endif()
