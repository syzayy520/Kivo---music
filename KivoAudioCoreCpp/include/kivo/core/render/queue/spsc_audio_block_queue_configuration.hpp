#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>

namespace kivo::core::render {

struct SpscAudioBlockQueueConfiguration {
    static constexpr uint32_t kMinimumSlotCount = 2;
    static constexpr uint32_t kMaximumSlotCount = 4096;
    static constexpr size_t kMaximumStorageBytes = 256u * 1024u * 1024u;

    uint32_t slot_count{0};
    size_t maximum_bytes_per_slot{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        if (slot_count < kMinimumSlotCount
            || slot_count > kMaximumSlotCount
            || maximum_bytes_per_slot == 0) {
            return false;
        }
        if (maximum_bytes_per_slot
            > std::numeric_limits<size_t>::max() / slot_count) {
            return false;
        }
        return maximum_bytes_per_slot * slot_count <= kMaximumStorageBytes;
    }

    [[nodiscard]] constexpr size_t storage_bytes() const noexcept {
        return is_valid() ? maximum_bytes_per_slot * slot_count : 0;
    }

    [[nodiscard]] constexpr bool operator==(
        const SpscAudioBlockQueueConfiguration&) const noexcept = default;
};

} // namespace kivo::core::render
