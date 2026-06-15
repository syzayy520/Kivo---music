#include "platform/windows/wasapi/renderer/publication/wasapi_renderer_publication.hpp"

#include <limits>

namespace kivo::platform::windows::wasapi::detail {

namespace {

[[nodiscard]] uint8_t state_code(
    core::render::RenderLifecycleState state) noexcept {
    return static_cast<uint8_t>(state);
}

[[nodiscard]] uint8_t failure_code(
    core::render::RenderFailure failure) noexcept {
    return static_cast<uint8_t>(failure);
}

[[nodiscard]] core::contract::FrameCount saturating_add(
    core::contract::FrameCount left,
    core::contract::FrameCount right) noexcept {
    const auto maximum = std::numeric_limits<core::contract::FrameCount>::max();
    return right > maximum - left ? maximum : left + right;
}

} // namespace

void WasapiRendererPublication::publish(
    const core::render::RenderSnapshot& snapshot) noexcept {
    const auto start = sequence_.fetch_add(1, std::memory_order_acq_rel) + 1;

    state_.store(state_code(snapshot.state), std::memory_order_relaxed);
    sample_format_.store(
        static_cast<uint8_t>(snapshot.accepted_format.format.sample_format),
        std::memory_order_relaxed);
    channel_layout_.store(
        static_cast<uint8_t>(snapshot.accepted_format.format.channel_layout),
        std::memory_order_relaxed);
    frame_layout_.store(
        static_cast<uint8_t>(snapshot.accepted_format.format.frame_layout),
        std::memory_order_relaxed);
    channel_mask_.store(
        snapshot.accepted_format.format.channel_mask.value,
        std::memory_order_relaxed);
    sample_rate_.store(
        snapshot.accepted_format.format.sample_rate,
        std::memory_order_relaxed);
    bits_per_sample_.store(
        snapshot.accepted_format.format.bits_per_sample,
        std::memory_order_relaxed);
    container_bits_per_sample_.store(
        snapshot.accepted_format.format.container_bits_per_sample,
        std::memory_order_relaxed);
    buffer_capacity_frames_.store(
        snapshot.buffer_capacity_frames,
        std::memory_order_relaxed);
    buffered_frames_.store(snapshot.buffered_frames, std::memory_order_relaxed);
    total_accepted_frames_.store(
        snapshot.total_accepted_frames,
        std::memory_order_relaxed);
    stream_generation_.store(
        snapshot.generations.stream.value(),
        std::memory_order_relaxed);
    seek_generation_.store(
        snapshot.generations.seek.value(),
        std::memory_order_relaxed);
    flush_generation_.store(
        snapshot.generations.flush.value(),
        std::memory_order_relaxed);
    device_generation_.store(
        snapshot.generations.device.value(),
        std::memory_order_relaxed);
    last_failure_.store(
        failure_code(snapshot.last_failure),
        std::memory_order_relaxed);

    sequence_.store(start + 1, std::memory_order_release);
}

core::render::RenderSnapshot WasapiRendererPublication::read() const noexcept {
    while (true) {
        const auto before = sequence_.load(std::memory_order_acquire);
        if ((before & 1u) != 0u) {
            continue;
        }

        core::render::RenderSnapshot snapshot{};
        snapshot.state = static_cast<core::render::RenderLifecycleState>(
            state_.load(std::memory_order_relaxed));
        snapshot.accepted_format.format.sample_format =
            static_cast<core::contract::SampleFormat>(
                sample_format_.load(std::memory_order_relaxed));
        snapshot.accepted_format.format.channel_layout =
            static_cast<core::contract::ChannelLayout>(
                channel_layout_.load(std::memory_order_relaxed));
        snapshot.accepted_format.format.frame_layout =
            static_cast<core::contract::FrameLayout>(
                frame_layout_.load(std::memory_order_relaxed));
        snapshot.accepted_format.format.channel_mask = core::contract::ChannelMask{
            channel_mask_.load(std::memory_order_relaxed)};
        snapshot.accepted_format.format.sample_rate =
            sample_rate_.load(std::memory_order_relaxed);
        snapshot.accepted_format.format.bits_per_sample =
            bits_per_sample_.load(std::memory_order_relaxed);
        snapshot.accepted_format.format.container_bits_per_sample =
            container_bits_per_sample_.load(std::memory_order_relaxed);
        snapshot.buffer_capacity_frames =
            buffer_capacity_frames_.load(std::memory_order_relaxed);
        snapshot.buffered_frames =
            buffered_frames_.load(std::memory_order_relaxed);
        snapshot.total_accepted_frames =
            total_accepted_frames_.load(std::memory_order_relaxed);
        snapshot.generations.stream.id = core::contract::GenerationId::from_value(
            stream_generation_.load(std::memory_order_relaxed));
        snapshot.generations.seek.id = core::contract::GenerationId::from_value(
            seek_generation_.load(std::memory_order_relaxed));
        snapshot.generations.flush.id = core::contract::GenerationId::from_value(
            flush_generation_.load(std::memory_order_relaxed));
        snapshot.generations.device.id = core::contract::GenerationId::from_value(
            device_generation_.load(std::memory_order_relaxed));
        snapshot.last_failure = static_cast<core::render::RenderFailure>(
            last_failure_.load(std::memory_order_relaxed));

        const auto after = sequence_.load(std::memory_order_acquire);
        if (before == after && (after & 1u) == 0u) {
            return snapshot;
        }
    }
}

void WasapiRendererPublication::sync_into(
    core::render::RenderSnapshot& snapshot) const noexcept {
    const auto current = read();
    snapshot.buffered_frames = current.buffered_frames;
    snapshot.total_accepted_frames = current.total_accepted_frames;
    snapshot.last_failure = current.last_failure;
    if (current.state == core::render::RenderLifecycleState::Failed) {
        snapshot.state = current.state;
    }
}

void WasapiRendererPublication::publish_buffered_frames(
    core::contract::FrameCount buffered_frames) noexcept {
    buffered_frames_.store(buffered_frames, std::memory_order_release);
}

void WasapiRendererPublication::add_accepted_frames(
    core::contract::FrameCount frames) noexcept {
    auto current = total_accepted_frames_.load(std::memory_order_relaxed);
    while (true) {
        const auto next = saturating_add(current, frames);
        if (total_accepted_frames_.compare_exchange_weak(
                current,
                next,
                std::memory_order_release,
                std::memory_order_relaxed)) {
            return;
        }
    }
}

void WasapiRendererPublication::publish_failure(
    core::render::RenderFailure failure) noexcept {
    last_failure_.store(failure_code(failure), std::memory_order_release);
}

void WasapiRendererPublication::publish_failed_state(
    core::render::RenderFailure failure) noexcept {
    const auto start = sequence_.fetch_add(1, std::memory_order_acq_rel) + 1;
    state_.store(
        state_code(core::render::RenderLifecycleState::Failed),
        std::memory_order_relaxed);
    last_failure_.store(failure_code(failure), std::memory_order_relaxed);
    sequence_.store(start + 1, std::memory_order_release);
}

} // namespace kivo::platform::windows::wasapi::detail
