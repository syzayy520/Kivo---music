#include "kivo/testing/render/renderer/fake_renderer.hpp"

#include <algorithm>
#include <limits>

namespace kivo::testing::render {

using core::contract::FrameCount;
using core::render::RenderFailure;
using core::render::RenderLifecycleState;

FakeConsumeResult FakeRenderer::consume(FrameCount requested_frames) noexcept {
    const auto ordinal = ++consume_count_;
    if (snapshot_.state != RenderLifecycleState::Started
        && snapshot_.state != RenderLifecycleState::Draining) {
        record(
            RenderObservationOperation::Consume,
            RenderFailure::InvalidState,
            requested_frames,
            0,
            ordinal);
        return {requested_frames, 0, false};
    }

    const auto forced_underrun =
        take_fault(RenderFaultKind::ForcedUnderrun, RenderFaultTrigger::NextConsume);
    const auto consumed_frames = std::min(requested_frames, snapshot_.buffered_frames);
    const auto underrun = forced_underrun || requested_frames > snapshot_.buffered_frames;
    snapshot_.buffered_frames -= consumed_frames;
    clock_.advance(duration_for_frames(consumed_frames));

    if (underrun) {
        snapshot_.last_failure = RenderFailure::Underrun;
    }
    if (snapshot_.state == RenderLifecycleState::Draining
        && snapshot_.buffered_frames == 0) {
        snapshot_.state = RenderLifecycleState::Stopped;
    }

    record(
        RenderObservationOperation::Consume,
        underrun ? RenderFailure::Underrun : RenderFailure::None,
        requested_frames,
        consumed_frames,
        ordinal);
    return {requested_frames, consumed_frames, underrun};
}

std::chrono::nanoseconds FakeRenderer::duration_for_frames(FrameCount frames) const noexcept {
    const auto sample_rate = snapshot_.accepted_format.format.sample_rate;
    if (sample_rate == 0 || frames == 0) {
        return std::chrono::nanoseconds{0};
    }

    constexpr uint64_t nanoseconds_per_second = 1'000'000'000;
    const auto whole_seconds = frames / sample_rate;
    const auto remaining_frames = frames % sample_rate;
    const auto maximum = static_cast<uint64_t>(std::numeric_limits<int64_t>::max());
    if (whole_seconds > maximum / nanoseconds_per_second) {
        return std::chrono::nanoseconds::max();
    }

    const auto whole_nanoseconds = whole_seconds * nanoseconds_per_second;
    const auto fractional_nanoseconds =
        remaining_frames * nanoseconds_per_second / sample_rate;
    if (fractional_nanoseconds > maximum - whole_nanoseconds) {
        return std::chrono::nanoseconds::max();
    }
    return std::chrono::nanoseconds{
        static_cast<int64_t>(whole_nanoseconds + fractional_nanoseconds)};
}

} // namespace kivo::testing::render
