#include "../fixture/ffmpeg_decode_test_runner.hpp"
#include "ffmpeg_decode_test_cases.hpp"

int main(int argc, char** argv) {
    std::cout << "=== FFmpeg Decode Adapter Tests ===\n\n";
    if (argc != 2) {
        std::cerr << "Expected fixture directory argument.\n";
        return 2;
    }

    FfmpegDecodeTestRunner runner{{argv[1]}};
    run_ffmpeg_decode_fixture_tests(runner);
    run_ffmpeg_policy_and_corruption_tests(runner);
    run_ffmpeg_seek_flush_tests(runner);

    std::cout << "\n=== " << runner.tests_passed << "/" << runner.tests_run
              << " passed ===\n";
    return runner.exit_code();
}
