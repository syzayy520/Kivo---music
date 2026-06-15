#pragma once

#include <atomic>
#include <cstdint>

#include "kivo/core/render/snapshot/render_snapshot.hpp"

namespace kivo::platform::windows::wasapi::detail {

class WasapiRendererPublication final {
public:
    void publish(const core::render::RenderSnapshot& snapshot) noexcept;
    [[nodiscard]] core::render::RenderSnapshot read() const noexcept;
    void sync_into(core::render::RenderSnapshot& snapshot) const noexcept;

    void publish_buffered_frames(
        core::contract::FrameCount buffered_frames) noexcept;
    void add_accepted_frames(core::contract::FrameCount frames) noexcept;
    void publish_failure(core::render::RenderFailure failure) noexcept;
    void publish_failed_state(core::render::RenderFailure failure) noexcept;

private:
    std::atomic<uint64_t> sequence_{0};
    std::atomic<uint8_t> state_{
        static_cast<uint8_t>(core::render::RenderLifecycleState::Closed)};
    std::atomic<uint8_t> sample_format_{0};
    std::atomic<uint8_t> channel_layout_{0};
    std::atomic<uint8_t> frame_layout_{0};
    std::atomic<uint32_t> channel_mask_{0};
    std::atomic<uint32_t> sample_rate_{0};
    std::atomic<uint8_t> bits_per_sample_{0};
    std::atomic<uint8_t> container_bits_per_sample_{0};
    std::atomic<core::contract::FrameCount> buffer_capacity_frames_{0};
    std::atomic<core::contract::FrameCount> buffered_frames_{0};
    std::atomic<core::contract::FrameCount> total_accepted_frames_{0};
    std::atomic<uint64_t> stream_generation_{0};
    std::atomic<uint64_t> seek_generation_{0};
    std::atomic<uint64_t> flush_generation_{0};
    std::atomic<uint64_t> device_generation_{0};
    std::atomic<uint8_t> last_failure_{
        static_cast<uint8_t>(core::render::RenderFailure::None)};
};

} // namespace kivo::platform::windows::wasapi::detail
