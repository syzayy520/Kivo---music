#include "kivo/core/render/pump/render_pump.hpp"

namespace kivo::core::render {

namespace {

[[nodiscard]] constexpr bool is_stale_generation(
    RenderFailure failure) noexcept {
    return failure == RenderFailure::StaleStreamGeneration
        || failure == RenderFailure::StaleSeekGeneration
        || failure == RenderFailure::StaleFlushGeneration
        || failure == RenderFailure::StaleDeviceGeneration;
}

} // namespace

RenderPump::RenderPump(
    SpscAudioBlockQueue& queue,
    RenderWriteBoundary& boundary) noexcept
    : queue_(queue), boundary_(boundary) {}

RenderPumpResult RenderPump::step() noexcept {
    const auto block = queue_.try_peek();
    if (!block) {
        return {
            queue_.producer_closed()
                ? RenderPumpDisposition::QueueDrained
                : RenderPumpDisposition::Idle
        };
    }

    const auto result = boundary_.write(*block);
    switch (result.disposition()) {
    case RenderWriteDisposition::Full:
    case RenderWriteDisposition::Partial:
        if (!queue_.consume_front(result.accepted_frames())) {
            return {
                RenderPumpDisposition::Failed,
                RenderFailure::BoundaryFailure,
                block->frame_count
            };
        }
        return {
            block->end_of_stream
                    && result.accepted_frames() == block->frame_count
                ? RenderPumpDisposition::EndOfStream
                : RenderPumpDisposition::Progress,
            RenderFailure::None,
            block->frame_count,
            result.accepted_frames()
        };

    case RenderWriteDisposition::Delayed:
        return {
            RenderPumpDisposition::Delayed,
            RenderFailure::None,
            block->frame_count,
            0,
            result.retry_after()
        };

    case RenderWriteDisposition::Rejected:
        if (is_stale_generation(result.failure())) {
            if (!queue_.consume_front(block->frame_count)) {
                return {
                    RenderPumpDisposition::Failed,
                    RenderFailure::BoundaryFailure,
                    block->frame_count
                };
            }
            return {
                RenderPumpDisposition::DiscardedStale,
                result.failure(),
                block->frame_count
            };
        }
        return {
            RenderPumpDisposition::Rejected,
            result.failure(),
            block->frame_count
        };

    case RenderWriteDisposition::Failed:
        return {
            RenderPumpDisposition::Failed,
            result.failure(),
            block->frame_count
        };
    }

    return {
        RenderPumpDisposition::Failed,
        RenderFailure::BoundaryFailure,
        block->frame_count
    };
}

} // namespace kivo::core::render
