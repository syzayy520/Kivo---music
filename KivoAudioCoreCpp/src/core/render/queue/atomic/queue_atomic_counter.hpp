#pragma once

#include <atomic>
#include <limits>

namespace kivo::core::render::detail {

template <typename Value>
void saturating_increment(
    std::atomic<Value>& value,
    Value amount = 1) noexcept {
    auto current = value.load(std::memory_order_relaxed);
    while (true) {
        const auto maximum = std::numeric_limits<Value>::max();
        const auto next =
            amount > maximum - current ? maximum : current + amount;
        if (value.compare_exchange_weak(
                current,
                next,
                std::memory_order_relaxed,
                std::memory_order_relaxed)) {
            return;
        }
    }
}

template <typename Value>
void saturating_decrement(
    std::atomic<Value>& value,
    Value amount) noexcept {
    auto current = value.load(std::memory_order_relaxed);
    while (true) {
        const auto next = amount > current ? Value{0} : current - amount;
        if (value.compare_exchange_weak(
                current,
                next,
                std::memory_order_relaxed,
                std::memory_order_relaxed)) {
            return;
        }
    }
}

template <typename Value>
void update_maximum(
    std::atomic<Value>& value,
    Value candidate) noexcept {
    auto current = value.load(std::memory_order_relaxed);
    while (candidate > current
        && !value.compare_exchange_weak(
            current,
            candidate,
            std::memory_order_relaxed,
            std::memory_order_relaxed)) {
    }
}

} // namespace kivo::core::render::detail
