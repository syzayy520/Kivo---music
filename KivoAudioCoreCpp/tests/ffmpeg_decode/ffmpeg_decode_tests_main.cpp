#include "ffmpeg_decode_tests_main.hpp"

void run_ffmpeg_session_tests(FfmpegDecodeTestRunner& runner);

int main(int argc, char** argv) {
    std::cout << "=== FFmpeg Decode Adapter Tests ===\n\n";
    if (argc != 2) {
        std::cerr << "Expected fixture directory argument.\n";
        return 2;
    }

    FfmpegDecodeTestRunner runner{{argv[1]}};
    run_ffmpeg_session_tests(runner);

    std::cout << "\n=== " << runner.tests_passed << "/" << runner.tests_run
              << " passed ===\n";
    return runner.exit_code();
}
