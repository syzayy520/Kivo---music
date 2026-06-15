#pragma once

#include <atomic>
#include <chrono>
#include <cstdint>

namespace kivo::testing::time {

class ManualClock {
public:
    using duration = std::chrono::nanoseconds;
    using time_point = std::chrono::time_point<std::chrono::steady_clock, duration>;

    ManualClock() noexcept = default;

    [[nodiscard]] time_point now() const noexcept;
    void reset(time_point value = time_point{}) noexcept;
    void advance(duration delta) noexcept;

private:
    std::atomic<int64_t> nanoseconds_{0};
};

} // namespace kivo::testing::time
