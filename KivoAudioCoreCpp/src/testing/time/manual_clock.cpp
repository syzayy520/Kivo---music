#include "kivo/testing/time/manual_clock.hpp"

#include <algorithm>
#include <limits>

namespace kivo::testing::time {

ManualClock::time_point ManualClock::now() const noexcept {
    return time_point{duration{nanoseconds_.load(std::memory_order_relaxed)}};
}

void ManualClock::reset(time_point value) noexcept {
    const auto count = std::max<int64_t>(0, value.time_since_epoch().count());
    nanoseconds_.store(count, std::memory_order_relaxed);
}

void ManualClock::advance(duration delta) noexcept {
    if (delta.count() <= 0) {
        return;
    }

    auto current = nanoseconds_.load(std::memory_order_relaxed);
    while (true) {
        const auto maximum = std::numeric_limits<int64_t>::max();
        const auto next = delta.count() > maximum - current
            ? maximum
            : current + delta.count();
        if (nanoseconds_.compare_exchange_weak(
                current,
                next,
                std::memory_order_relaxed,
                std::memory_order_relaxed)) {
            return;
        }
    }
}

} // namespace kivo::testing::time
