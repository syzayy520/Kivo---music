// =============================================================================
// Kivo Music Qt - tests/CMakeLists.txt
// 职责: 单元测试构建配置
// =============================================================================

cmake_minimum_required(VERSION 3.28)

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
    gtest_discover_tests(${name})
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
