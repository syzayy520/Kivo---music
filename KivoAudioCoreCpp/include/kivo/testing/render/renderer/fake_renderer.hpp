#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <optional>

#include "kivo/core/render/boundary/render_boundary.hpp"
#include "kivo/testing/render/configuration/fake_renderer_configuration.hpp"
#include "kivo/testing/render/fault/scheduled_render_fault.hpp"
#include "kivo/testing/render/observation/bounded_observation_history.hpp"
#include "kivo/testing/time/manual_clock.hpp"

namespace kivo::testing::render {

struct FakeConsumeResult {
    core::contract::FrameCount requested_frames{0};
    core::contract::FrameCount consumed_frames{0};
    bool underrun{false};

    [[nodiscard]] constexpr bool operator==(const FakeConsumeResult&) const noexcept = default;
};

class FakeRenderer final : public core::render::RenderBoundary {
public:
    static constexpr size_t kMaximumScheduledFaults = 32;

    FakeRenderer(
        FakeRendererConfiguration configuration,
        time::ManualClock& clock) noexcept;

    [[nodiscard]] core::render::RendererIdentity identity() const noexcept override;
    [[nodiscard]] core::render::RendererCapabilitySnapshot capabilities() const noexcept override;
    [[nodiscard]] core::render::RenderOpenResult open(
        const core::render::RenderOpenRequest& request) noexcept override;
    [[nodiscard]] core::render::RenderControlResult start() noexcept override;
    [[nodiscard]] core::render::RenderControlResult flush(
        const core::render::RenderFlushRequest& request) noexcept override;
    [[nodiscard]] core::render::RenderControlResult drain(
        const core::render::RenderDrainRequest& request) noexcept override;
    [[nodiscard]] core::render::RenderControlResult reset(
        const core::render::RenderResetRequest& request) noexcept override;
    [[nodiscard]] core::render::RenderControlResult stop() noexcept override;
    [[nodiscard]] core::render::RenderControlResult close() noexcept override;
    [[nodiscard]] core::render::RenderSnapshot snapshot() const noexcept override;
    [[nodiscard]] core::render::RenderWriteResult write(
        const core::render::AudioBlockView& block) noexcept override;

    [[nodiscard]] bool schedule_fault(const ScheduledRenderFault& fault) noexcept;
    [[nodiscard]] FakeConsumeResult consume(
        core::contract::FrameCount requested_frames) noexcept;

    [[nodiscard]] size_t observation_count() const noexcept;
    [[nodiscard]] std::optional<RenderObservation> observation_at(size_t index) const noexcept;
    void clear_observations() noexcept;

    [[nodiscard]] uint64_t write_count() const noexcept;
    [[nodiscard]] uint64_t consume_count() const noexcept;

private:
    struct FaultSlot {
        ScheduledRenderFault fault{};
        bool active{false};
    };

    [[nodiscard]] core::render::RenderFailure generation_failure(
        const core::render::RenderGenerationSet& generations) const noexcept;
    [[nodiscard]] std::optional<ScheduledRenderFault> take_write_fault(
        const core::render::AudioBlockView& block,
        uint64_t write_ordinal) noexcept;
    [[nodiscard]] bool take_fault(
        RenderFaultKind kind,
        RenderFaultTrigger trigger) noexcept;
    [[nodiscard]] std::chrono::nanoseconds duration_for_frames(
        core::contract::FrameCount frames) const noexcept;
    void record(
        RenderObservationOperation operation,
        core::render::RenderFailure failure = core::render::RenderFailure::None,
        core::contract::FrameCount submitted_frames = 0,
        core::contract::FrameCount accepted_frames = 0,
        uint64_t ordinal = 0) noexcept;

    FakeRendererConfiguration configuration_;
    time::ManualClock& clock_;
    BoundedObservationHistory observations_;
    std::array<FaultSlot, kMaximumScheduledFaults> faults_{};
    core::render::RenderSnapshot snapshot_{};
    uint64_t write_count_{0};
    uint64_t consume_count_{0};
};

} // namespace kivo::testing::render
