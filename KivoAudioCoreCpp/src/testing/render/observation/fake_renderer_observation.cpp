#include "kivo/testing/render/renderer/fake_renderer.hpp"

namespace kivo::testing::render {

core::render::RenderSnapshot FakeRenderer::snapshot() const noexcept {
    return snapshot_;
}

size_t FakeRenderer::observation_count() const noexcept {
    return observations_.size();
}

std::optional<RenderObservation> FakeRenderer::observation_at(size_t index) const noexcept {
    return observations_.at(index);
}

void FakeRenderer::clear_observations() noexcept {
    observations_.clear();
}

uint64_t FakeRenderer::write_count() const noexcept {
    return write_count_;
}

uint64_t FakeRenderer::consume_count() const noexcept {
    return consume_count_;
}

void FakeRenderer::record(
    RenderObservationOperation operation,
    core::render::RenderFailure failure,
    core::contract::FrameCount submitted_frames,
    core::contract::FrameCount accepted_frames,
    uint64_t ordinal) noexcept {
    observations_.push({
        operation,
        clock_.now().time_since_epoch(),
        snapshot_.state,
        failure,
        submitted_frames,
        accepted_frames,
        ordinal,
        snapshot_.generations
    });
}

} // namespace kivo::testing::render
