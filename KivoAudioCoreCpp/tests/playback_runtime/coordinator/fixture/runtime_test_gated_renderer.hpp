#pragma once

#include "kivo/testing/render/fake_renderer.hpp"

#include "runtime_test_call_gate.hpp"

namespace playback_runtime_test {

class RuntimeTestGatedRenderer final
    : public kivo::core::render::RenderBoundary {
public:
    RuntimeTestGatedRenderer(
        kivo::testing::render::FakeRendererConfiguration configuration,
        kivo::testing::time::ManualClock& clock,
        RuntimeTestCallGate* write_gate = nullptr,
        RuntimeTestCallGate* drain_gate = nullptr) noexcept
        : renderer_(configuration, clock),
          write_gate_(write_gate),
          drain_gate_(drain_gate) {}

    [[nodiscard]] kivo::core::render::RendererIdentity identity()
        const noexcept override {
        return renderer_.identity();
    }

    [[nodiscard]] kivo::core::render::RendererCapabilitySnapshot
    capabilities() const noexcept override {
        return renderer_.capabilities();
    }

    [[nodiscard]] kivo::core::render::RenderOpenResult open(
        const kivo::core::render::RenderOpenRequest& request)
        noexcept override {
        return renderer_.open(request);
    }

    [[nodiscard]] kivo::core::render::RenderControlResult start()
        noexcept override {
        return renderer_.start();
    }

    [[nodiscard]] kivo::core::render::RenderControlResult flush(
        const kivo::core::render::RenderFlushRequest& request)
        noexcept override {
        return renderer_.flush(request);
    }

    [[nodiscard]] kivo::core::render::RenderControlResult drain(
        const kivo::core::render::RenderDrainRequest& request)
        noexcept override {
        if (drain_gate_) {
            drain_gate_->enter_and_wait();
        }
        return renderer_.drain(request);
    }

    [[nodiscard]] kivo::core::render::RenderControlResult reset(
        const kivo::core::render::RenderResetRequest& request)
        noexcept override {
        return renderer_.reset(request);
    }

    [[nodiscard]] kivo::core::render::RenderControlResult stop()
        noexcept override {
        return renderer_.stop();
    }

    [[nodiscard]] kivo::core::render::RenderControlResult close()
        noexcept override {
        return renderer_.close();
    }

    [[nodiscard]] kivo::core::render::RenderSnapshot snapshot()
        const noexcept override {
        return renderer_.snapshot();
    }

    [[nodiscard]] kivo::core::render::RenderWriteResult write(
        const kivo::core::render::AudioBlockView& block)
        noexcept override {
        if (write_gate_) {
            write_gate_->enter_and_wait();
        }
        return renderer_.write(block);
    }

private:
    kivo::testing::render::FakeRenderer renderer_;
    RuntimeTestCallGate* write_gate_;
    RuntimeTestCallGate* drain_gate_;
};

} // namespace playback_runtime_test
