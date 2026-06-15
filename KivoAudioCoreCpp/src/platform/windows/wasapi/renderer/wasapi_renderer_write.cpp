#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include <algorithm>
#include <cstring>

namespace kivo::platform::windows::wasapi::detail {

core::render::RenderWriteResult WasapiRendererState::write(
    const core::render::AudioBlockView& block) noexcept {
    using core::contract::FrameCount;
    using core::render::RenderFailure;
    using core::render::RenderLifecycleState;
    using core::render::RenderWriteResult;

    const auto current = publication_.read();
    if (current.state != RenderLifecycleState::Started) {
        return RenderWriteResult::Rejected(block.frame_count, RenderFailure::InvalidState);
    }
    if (!block.is_valid()) {
        return RenderWriteResult::Rejected(block.frame_count, RenderFailure::InvalidBuffer);
    }
    if (block.format != current.accepted_format) {
        return RenderWriteResult::Rejected(
            block.frame_count,
            RenderFailure::UnsupportedFormat);
    }

    const auto generation_failure = stale_generation(
        block.generations,
        current.generations);
    if (generation_failure != RenderFailure::None) {
        return RenderWriteResult::Rejected(block.frame_count, generation_failure);
    }

    UINT32 padding = 0;
    auto result = audio_client_->GetCurrentPadding(&padding);
    if (FAILED(result)) {
        return fail_write(result, block.frame_count);
    }
    if (padding > current.buffer_capacity_frames) {
        return fail_write(AUDCLNT_E_BUFFER_SIZE_ERROR, block.frame_count);
    }
    if (padding == 0
        && has_submitted_since_start_.load(std::memory_order_relaxed)) {
        publication_.publish_failure(RenderFailure::Underrun);
    }

    const auto available = current.buffer_capacity_frames - padding;
    const auto accepted = std::min<FrameCount>(block.frame_count, available);
    if (accepted == 0) {
        return RenderWriteResult::Delayed(
            block.frame_count,
            std::chrono::milliseconds{1});
    }

    BYTE* destination = nullptr;
    result = render_client_->GetBuffer(static_cast<UINT32>(accepted), &destination);
    if (FAILED(result)) {
        return fail_write(result, block.frame_count);
    }

    const auto bytes_per_frame = block.format.format.bytes_per_frame();
    const auto byte_count = static_cast<size_t>(accepted) * bytes_per_frame;
    std::memcpy(destination, block.bytes.data(), byte_count);
    result = render_client_->ReleaseBuffer(static_cast<UINT32>(accepted), 0);
    if (FAILED(result)) {
        return fail_write(result, block.frame_count);
    }

    publication_.publish_buffered_frames(padding + accepted);
    publication_.add_accepted_frames(accepted);
    has_submitted_since_start_.store(true, std::memory_order_relaxed);
    return accepted == block.frame_count
        ? RenderWriteResult::Full(block.frame_count)
        : RenderWriteResult::Partial(block.frame_count, accepted);
}

} // namespace kivo::platform::windows::wasapi::detail
