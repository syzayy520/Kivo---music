#pragma once

#include <memory>

#include "kivo/core/playback/gapless/coordinator/gapless_transition_coordinator.hpp"
#include "kivo/core/playback/pipeline/producer/decode_render_queue_producer.hpp"

namespace kivo::core::playback {

[[nodiscard]] std::unique_ptr<DecodeRenderQueueProducer>
make_gapless_producer(
    render::SpscAudioBlockQueue& queue,
    const GaplessTrackPrepareRequest& request) noexcept;

class GaplessTransitionCoordinator::Impl {
public:
    Impl(
        render::SpscAudioBlockQueue& queue,
        GaplessTrackPlan current_plan,
        std::unique_ptr<DecodeRenderQueueProducer> current) noexcept;

    [[nodiscard]] GaplessPrepareResult prepare_next(
        const GaplessTrackPrepareRequest& request) noexcept;
    [[nodiscard]] GaplessCoordinatorResult step() noexcept;
    [[nodiscard]] GaplessCoordinatorSnapshot snapshot() const noexcept;

private:
    [[nodiscard]] GaplessCoordinatorResult map(
        const DecodeRenderQueueProducerResult& result,
        GaplessCoordinatorDisposition override_disposition) noexcept;
    void refresh() noexcept;
    static void saturating_add(
        contract::FrameCount& target,
        contract::FrameCount amount) noexcept;

    render::SpscAudioBlockQueue& queue_;
    GaplessTrackPlan current_plan_{};
    GaplessTrackPlan next_plan_{};
    std::unique_ptr<DecodeRenderQueueProducer> current_{};
    std::unique_ptr<DecodeRenderQueueProducer> next_{};
    contract::FrameCount completed_decoded_{0};
    contract::FrameCount completed_queued_{0};
    GaplessCoordinatorSnapshot snapshot_{};
};

} // namespace kivo::core::playback
