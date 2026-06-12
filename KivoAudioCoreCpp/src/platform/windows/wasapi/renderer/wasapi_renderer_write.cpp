#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

#include <algorithm>
#include <cstring>
#include <limits>

namespace kivo::platform::windows::wasapi::detail {

namespace {

[[nodiscard]] core::contract::FrameCount saturating_add(
    core::contract::FrameCount left,
    core::contract::FrameCount right) noexcept {
    const auto maximum = std::numeric_limits<core::contract::FrameCount>::max();
    return right > maximum - left ? maximum : left + right;
}

} // namespace

core::render::RenderWriteResult WasapiRendererState::write(
    const core::render::AudioBlockView& block) noexcept {
    using core::contract::FrameCount;
    using core::render::RenderFailure;
    using core::render::RenderLifecycleState;
    using core::render::RenderWriteResult;

    if (detect_endpoint_change()) {
        return RenderWriteResult::Failed(
            block.frame_count,
            RenderFailure::DeviceLost);
    }
    if (snapshot_.state != RenderLifecycleState::Started) {
        return RenderWriteResult::Rejected(block.frame_count, RenderFailure::InvalidState);
    }
    if (!block.is_valid()) {
        return RenderWriteResult::Rejected(block.frame_count, RenderFailure::InvalidBuffer);
    }
    if (block.format != snapshot_.accepted_format) {
        return RenderWriteResult::Rejected(
            block.frame_count,
            RenderFailure::UnsupportedFormat);
    }

    const auto generation_failure = stale_generation(block.generations);
    if (generation_failure != RenderFailure::None) {
        return RenderWriteResult::Rejected(block.frame_count, generation_failure);
    }

    UINT32 padding = 0;
    auto result = audio_client_->GetCurrentPadding(&padding);
    if (FAILED(result)) {
        return fail_write(result, block.frame_count);
    }
    if (padding > snapshot_.buffer_capacity_frames) {
        return fail_write(AUDCLNT_E_BUFFER_SIZE_ERROR, block.frame_count);
    }
    if (padding == 0 && has_submitted_since_start_) {
        ++diagnostics_.underrun_count;
        snapshot_.last_failure = RenderFailure::Underrun;
    }

    const auto available = snapshot_.buffer_capacity_frames - padding;
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

    snapshot_.buffered_frames = padding + accepted;
    snapshot_.total_accepted_frames =
        saturating_add(snapshot_.total_accepted_frames, accepted);
    has_submitted_since_start_ = true;
    return accepted == block.frame_count
        ? RenderWriteResult::Full(block.frame_count)
        : RenderWriteResult::Partial(block.frame_count, accepted);
}

} // namespace kivo::platform::windows::wasapi::detail
