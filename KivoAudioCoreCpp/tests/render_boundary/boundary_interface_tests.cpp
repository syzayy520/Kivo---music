#include "render_boundary_tests_main.hpp"

#include <type_traits>
#include <utility>

#include "kivo/core/render/boundary/render_boundary.hpp"

namespace {

using namespace kivo::core;

class StubRenderBoundary final : public render::RenderBoundary {
public:
    [[nodiscard]] render::RendererIdentity identity() const noexcept override {
        return {1};
    }

    [[nodiscard]] render::RendererCapabilitySnapshot capabilities() const noexcept override {
        return {};
    }

    [[nodiscard]] render::RenderOpenResult open(
        const render::RenderOpenRequest&) noexcept override {
        return render::RenderOpenResult::Rejected(render::RenderFailure::UnsupportedFormat);
    }

    [[nodiscard]] render::RenderControlResult start() noexcept override {
        return render::RenderControlResult::Succeeded(render::RenderLifecycleState::Started);
    }

    [[nodiscard]] render::RenderControlResult flush(
        const render::RenderFlushRequest&) noexcept override {
        return render::RenderControlResult::Succeeded(render::RenderLifecycleState::Started);
    }

    [[nodiscard]] render::RenderControlResult drain(
        const render::RenderDrainRequest&) noexcept override {
        return render::RenderControlResult::Succeeded(render::RenderLifecycleState::Stopped);
    }

    [[nodiscard]] render::RenderControlResult reset(
        const render::RenderResetRequest&) noexcept override {
        return render::RenderControlResult::Succeeded(render::RenderLifecycleState::Open);
    }

    [[nodiscard]] render::RenderControlResult stop() noexcept override {
        return render::RenderControlResult::Succeeded(render::RenderLifecycleState::Stopped);
    }

    [[nodiscard]] render::RenderControlResult close() noexcept override {
        return render::RenderControlResult::Succeeded(render::RenderLifecycleState::Closed);
    }

    [[nodiscard]] render::RenderSnapshot snapshot() const noexcept override {
        return {};
    }

    [[nodiscard]] render::RenderWriteResult write(
        const render::AudioBlockView& block) noexcept override {
        return render::RenderWriteResult::Rejected(
            block.frame_count,
            render::RenderFailure::InvalidState);
    }
};

static_assert(std::is_abstract_v<render::RenderControlBoundary>);
static_assert(std::is_abstract_v<render::RenderWriteBoundary>);
static_assert(std::is_abstract_v<render::RenderBoundary>);
static_assert(std::is_final_v<StubRenderBoundary>);
static_assert(noexcept(std::declval<render::RenderWriteBoundary&>().write(
    std::declval<const render::AudioBlockView&>())));
static_assert(noexcept(std::declval<render::RenderControlBoundary&>().start()));

void combined_boundary_exposes_control_and_write_surfaces() {
    StubRenderBoundary renderer;
    render::RenderControlBoundary& control = renderer;
    render::RenderWriteBoundary& write = renderer;

    RENDER_ASSERT(control.identity() == render::RendererIdentity{1});
    RENDER_ASSERT(
        control.start().disposition() == render::RenderControlDisposition::Succeeded);

    const auto result = write.write({});
    RENDER_ASSERT(result.disposition() == render::RenderWriteDisposition::Rejected);
    RENDER_ASSERT(result.failure() == render::RenderFailure::InvalidState);
}

void closed_snapshot_is_valid() {
    const StubRenderBoundary renderer;
    const auto snapshot = renderer.snapshot();
    RENDER_ASSERT(snapshot.state == render::RenderLifecycleState::Closed);
    RENDER_ASSERT(snapshot.is_valid());
}

void failed_before_open_snapshot_is_valid() {
    render::RenderSnapshot snapshot{};
    snapshot.state = render::RenderLifecycleState::Failed;
    snapshot.last_failure = render::RenderFailure::DeviceLost;
    RENDER_ASSERT(snapshot.is_valid());
}

} // namespace

void run_render_boundary_interface_tests(RenderBoundaryTestRunner& runner) {
    runner.run(
        "combined_boundary_exposes_control_and_write_surfaces",
        combined_boundary_exposes_control_and_write_surfaces);
    runner.run("closed_snapshot_is_valid", closed_snapshot_is_valid);
    runner.run("failed_before_open_snapshot_is_valid", failed_before_open_snapshot_is_valid);
}
