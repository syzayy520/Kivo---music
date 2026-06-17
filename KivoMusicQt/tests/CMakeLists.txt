# =============================================================================
# Kivo Music Qt - tests/CMakeLists.txt
# 职责: 单元测试构建配置
# =============================================================================

cmake_minimum_required(VERSION 3.28)

project(KivoMusicQtTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The tested classes (PlaybackMode, PlayQueue, EqPresetManager) are QObjects with
# Q_OBJECT, so moc must generate their meta-object code or linking fails with
# unresolved metaObject/qt_metacall/signal symbols. AUTOMOC moc's each sibling
# header (foo.hpp next to foo.cpp) automatically.
set(CMAKE_AUTOMOC ON)

# Qt6::Core is linked by the test targets below, so it must be found here. When
# this directory is configured standalone (the normal way the test suite is run),
# point CMAKE_PREFIX_PATH at the Qt install — KIVO_QT_ROOT or the project-local
# .qt/ fallback, mirroring the app's top-level CMakeLists.
if(NOT TARGET Qt6::Core)
    if(DEFINED ENV{KIVO_QT_ROOT} AND IS_DIRECTORY "$ENV{KIVO_QT_ROOT}")
        list(APPEND CMAKE_PREFIX_PATH "$ENV{KIVO_QT_ROOT}")
    elseif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../.qt/6.8.3/msvc2022_64")
        list(APPEND CMAKE_PREFIX_PATH
            "${CMAKE_CURRENT_SOURCE_DIR}/../.qt/6.8.3/msvc2022_64")
    endif()
    find_package(Qt6 REQUIRED COMPONENTS Core)
endif()

# Find Google Test (use FetchContent for portability)
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

enable_testing()

function(add_kivo_test name)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE
        GTest::gtest_main
        Qt6::Core
    )
    target_include_directories(${name} PRIVATE
        ${CMAKE_SOURCE_DIR}/..
    )
    # NOTE: gtest_discover_tests() is deliberately NOT used. It runs the freshly
    # built exe to enumerate cases, passing a discovery-file path through narrow
    # argv; under a non-ASCII source tree (…\开发\…) that path is mangled to "?"
    # and discovery aborts. Registering the binary as a single ctest sidesteps
    # that — gtest_main still runs every case. ENVIRONMENT_MODIFICATION prepends
    # Qt's DLL directory to PATH so the test loads Qt6Core.dll with no deploy step.
    add_test(NAME ${name} COMMAND ${name})
    set_tests_properties(${name} PROPERTIES
        ENVIRONMENT_MODIFICATION "PATH=path_list_prepend:$<TARGET_FILE_DIR:Qt6::Core>")
endfunction()

add_kivo_test(test_play_queue
    test_play_queue.cpp
    ../native/audio_core_bridge/queue/play_queue.cpp
)

add_kivo_test(test_playback_mode
    test_playback_mode.cpp
    ../native/audio_core_bridge/queue/playback_mode.cpp
)

add_kivo_test(test_eq_preset
    test_eq_preset.cpp
    ../native/audio_core_bridge/eq/eq_preset.cpp
)

add_kivo_test(test_lyrics_parser
    test_lyrics_parser.cpp
    ../native/audio_core_bridge/lyrics/lyrics_parser.cpp
)

add_kivo_test(test_field_swap_detector
    test_field_swap_detector.cpp
    ../native/metadata_intelligence/validation/metadata_value_validator.cpp
    ../native/metadata_intelligence/confidence/swap_detector.cpp
    ../native/metadata_intelligence/repair/swap_corrector.cpp
)

add_kivo_test(test_metadata_repair_pipeline
    test_metadata_repair_pipeline.cpp
    ../native/metadata_intelligence/validation/metadata_value_validator.cpp
    ../native/metadata_intelligence/confidence/swap_detector.cpp
    ../native/metadata_intelligence/repair/swap_corrector.cpp
    ../native/metadata_intelligence/report/metadata_repair_report.cpp
    ../native/metadata_intelligence/pipeline/metadata_repair_pipeline.cpp
)

add_kivo_test(test_metadata_repair_review_queue_basic
    test_metadata_repair_review_queue_basic.cpp
    ../native/metadata_intelligence/validation/metadata_value_validator.cpp
    ../native/metadata_intelligence/confidence/swap_detector.cpp
    ../native/metadata_intelligence/repair/swap_corrector.cpp
    ../native/metadata_intelligence/report/metadata_repair_report.cpp
    ../native/metadata_intelligence/pipeline/metadata_repair_pipeline.cpp
    ../native/metadata_intelligence/review/metadata_repair_review_queue.cpp
)

add_kivo_test(test_metadata_repair_review_queue_filter
    test_metadata_repair_review_queue_filter.cpp
    ../native/metadata_intelligence/validation/metadata_value_validator.cpp
    ../native/metadata_intelligence/confidence/swap_detector.cpp
    ../native/metadata_intelligence/repair/swap_corrector.cpp
    ../native/metadata_intelligence/report/metadata_repair_report.cpp
    ../native/metadata_intelligence/pipeline/metadata_repair_pipeline.cpp
    ../native/metadata_intelligence/review/metadata_repair_review_queue.cpp
)

add_kivo_test(test_metadata_repair_review_queue_sorting
    test_metadata_repair_review_queue_sorting.cpp
    ../native/metadata_intelligence/validation/metadata_value_validator.cpp
    ../native/metadata_intelligence/confidence/swap_detector.cpp
    ../native/metadata_intelligence/repair/swap_corrector.cpp
    ../native/metadata_intelligence/report/metadata_repair_report.cpp
    ../native/metadata_intelligence/pipeline/metadata_repair_pipeline.cpp
    ../native/metadata_intelligence/review/metadata_repair_review_queue.cpp
)

add_kivo_test(test_metadata_repair_review_queue_safety
    test_metadata_repair_review_queue_safety.cpp
    ../native/metadata_intelligence/validation/metadata_value_validator.cpp
    ../native/metadata_intelligence/confidence/swap_detector.cpp
    ../native/metadata_intelligence/repair/swap_corrector.cpp
    ../native/metadata_intelligence/report/metadata_repair_report.cpp
    ../native/metadata_intelligence/pipeline/metadata_repair_pipeline.cpp
    ../native/metadata_intelligence/review/metadata_repair_review_queue.cpp
)


