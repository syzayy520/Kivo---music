#include "gapless_transition_coordinator_state.hpp"

#include <new>
#include <utility>

namespace kivo::core::playback {

GaplessTransitionCoordinator::Impl::Impl(
    render::SpscAudioBlockQueue& queue,
    GaplessTrackPlan current_plan,
    std::unique_ptr<DecodeRenderQueueProducer> current) noexcept
    : queue_(queue),
      current_plan_(current_plan),
      current_(std::move(current)) {
    snapshot_.current_track_index = current_plan_.track_index;
}

GaplessCoordinatorSnapshot
GaplessTransitionCoordinator::Impl::snapshot() const noexcept {
    return snapshot_;
}

std::unique_ptr<GaplessTransitionCoordinator>
GaplessTransitionCoordinator::create(
    render::SpscAudioBlockQueue& queue,
    const GaplessTrackPrepareRequest& first_track) noexcept {
    if (!first_track.is_valid()) {
        return {};
    }
    auto producer = make_gapless_producer(queue, first_track);
    if (!producer) {
        return {};
    }
    auto impl = std::unique_ptr<Impl>{new (std::nothrow) Impl(
        queue,
        first_track.plan,
        std::move(producer))};
    return impl
        ? std::unique_ptr<GaplessTransitionCoordinator>{
            new (std::nothrow) GaplessTransitionCoordinator(std::move(impl))}
        : nullptr;
}

GaplessTransitionCoordinator::GaplessTransitionCoordinator(
    std::unique_ptr<Impl> impl) noexcept
    : impl_(std::move(impl)) {}

GaplessTransitionCoordinator::~GaplessTransitionCoordinator() = default;

GaplessPrepareResult GaplessTransitionCoordinator::prepare_next(
    const GaplessTrackPrepareRequest& next_track) noexcept {
    return impl_ ? impl_->prepare_next(next_track) : GaplessPrepareResult{};
}

GaplessCoordinatorResult GaplessTransitionCoordinator::step() noexcept {
    return impl_ ? impl_->step() : GaplessCoordinatorResult{};
}

GaplessCoordinatorSnapshot
GaplessTransitionCoordinator::snapshot() const noexcept {
    return impl_ ? impl_->snapshot() : GaplessCoordinatorSnapshot{};
}

} // namespace kivo::core::playback
