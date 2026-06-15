#pragma once

#include <filesystem>
#include <iostream>
#include <stdexcept>

struct FfmpegDecodeTestContext {
    std::filesystem::path fixture_directory{};
};

struct FfmpegDecodeTestRunner {
    explicit FfmpegDecodeTestRunner(FfmpegDecodeTestContext context)
        : context(std::move(context)) {}

    int tests_run{0};
    int tests_passed{0};
    FfmpegDecodeTestContext context{};

    void run(
        const char* name,
        void (*test_fn)(const FfmpegDecodeTestContext&)) {
        ++tests_run;
        std::cout << "  " << name << "... ";
        try {
            test_fn(context);
            ++tests_passed;
            std::cout << "PASS\n";
        } catch (const std::exception& exception) {
            std::cout << "FAIL: " << exception.what() << "\n";
        }
    }

    [[nodiscard]] int exit_code() const noexcept {
        return tests_run == tests_passed ? 0 : 1;
    }
};

#define FFMPEG_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            throw std::runtime_error("Assertion failed: " #condition); \
        } \
    } while (false)
