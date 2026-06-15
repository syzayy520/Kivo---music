#include "../fixture/fake_renderer_test_runner.hpp"

#include <chrono>
#include <limits>

#include "kivo/testing/time/manual_clock.hpp"

namespace {

using namespace std::chrono_literals;

void manual_clock_advances_only_explicitly() {
    kivo::testing::time::ManualClock clock;
    FAKE_ASSERT(clock.now().time_since_epoch() == 0ns);

    clock.advance(5ms);
    FAKE_ASSERT(clock.now().time_since_epoch() == 5ms);

    clock.advance(-1ms);
    FAKE_ASSERT(clock.now().time_since_epoch() == 5ms);
}

void manual_clock_reset_rejects_negative_time() {
    kivo::testing::time::ManualClock clock;
    clock.reset(kivo::testing::time::ManualClock::time_point{-10ns});
    FAKE_ASSERT(clock.now().time_since_epoch() == 0ns);
}

void manual_clock_saturates_instead_of_wrapping() {
    kivo::testing::time::ManualClock clock;
    const auto maximum = std::chrono::nanoseconds::max();
    clock.reset(kivo::testing::time::ManualClock::time_point{maximum - 1ns});
    clock.advance(10ns);
    FAKE_ASSERT(clock.now().time_since_epoch() == maximum);
}

} // namespace

void run_manual_clock_tests(FakeRendererTestRunner& runner) {
    runner.run("manual_clock_advances_only_explicitly", manual_clock_advances_only_explicitly);
    runner.run(
        "manual_clock_reset_rejects_negative_time",
        manual_clock_reset_rejects_negative_time);
    runner.run(
        "manual_clock_saturates_instead_of_wrapping",
        manual_clock_saturates_instead_of_wrapping);
}
