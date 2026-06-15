#include "../fixture/stability_test_runner.hpp"
#include "stability_test_cases.hpp"

int main(int argc, char** argv) {
    std::cout << "=== Commercial Stability Tests ===\n\n";
    if (argc != 2) {
        std::cerr << "Expected fixture directory argument.\n";
        return 2;
    }

    StabilityTestRunner runner{{argv[1]}};
    runner.run(
        "repeated_fake_renderer_lifecycle_is_bounded",
        repeated_fake_renderer_lifecycle_is_bounded);
    runner.run(
        "repeated_ffmpeg_lifecycle_has_bounded_resource_growth",
        repeated_ffmpeg_lifecycle_has_bounded_resource_growth);

    std::cout << "\n=== " << runner.tests_passed << "/" << runner.tests_run
              << " passed ===\n";
    return runner.exit_code();
}
