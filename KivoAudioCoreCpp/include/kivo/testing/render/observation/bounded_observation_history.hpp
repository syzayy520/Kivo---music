#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <optional>

#include "kivo/testing/render/observation/render_observation.hpp"

namespace kivo::testing::render {

class BoundedObservationHistory {
public:
    static constexpr size_t kMaximumCapacity = kMaximumObservationHistoryCapacity;

    explicit constexpr BoundedObservationHistory(size_t capacity = 64) noexcept
        : capacity_(std::clamp(capacity, size_t{1}, kMaximumCapacity)) {}

    void push(const RenderObservation& observation) noexcept {
        if (size_ < capacity_) {
            const auto index = (start_ + size_) % capacity_;
            entries_[index] = observation;
            ++size_;
            return;
        }

        entries_[start_] = observation;
        start_ = (start_ + 1) % capacity_;
    }

    [[nodiscard]] constexpr size_t size() const noexcept {
        return size_;
    }

    [[nodiscard]] constexpr size_t capacity() const noexcept {
        return capacity_;
    }

    [[nodiscard]] std::optional<RenderObservation> at(size_t index) const noexcept {
        if (index >= size_) {
            return std::nullopt;
        }
        return entries_[(start_ + index) % capacity_];
    }

    void clear() noexcept {
        start_ = 0;
        size_ = 0;
    }

private:
    std::array<RenderObservation, kMaximumCapacity> entries_{};
    size_t capacity_{64};
    size_t start_{0};
    size_t size_{0};
};

} // namespace kivo::testing::render
