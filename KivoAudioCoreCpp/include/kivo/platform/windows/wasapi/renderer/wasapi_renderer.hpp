#pragma once

#include <chrono>
#include <memory>

#include "kivo/core/render/boundary/render_boundary.hpp"
#include "kivo/platform/windows/wasapi/diagnostics/wasapi_diagnostics.hpp"
#include "kivo/platform/windows/wasapi/result/wasapi_wait_result.hpp"

namespace kivo::platform::windows::wasapi {

namespace detail {
class WasapiRendererState;
}

class WasapiRenderer final : public core::render::RenderBoundary {
public:
    WasapiRenderer();
    ~WasapiRenderer() override;

    WasapiRenderer(const WasapiRenderer&) = delete;
    WasapiRenderer& operator=(const WasapiRenderer&) = delete;
    WasapiRenderer(WasapiRenderer&&) = delete;
    WasapiRenderer& operator=(WasapiRenderer&&) = delete;

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

    [[nodiscard]] static constexpr WasapiModeCapabilities supported_modes() noexcept {
        return {};
    }

    [[nodiscard]] WasapiWaitResult wait_until_writable(
        std::chrono::milliseconds timeout) noexcept;
    [[nodiscard]] bool is_started_for_worker() const noexcept;
    [[nodiscard]] WasapiDiagnostics diagnostics() const noexcept;

private:
    std::unique_ptr<detail::WasapiRendererState> impl_;
};

} // namespace kivo::platform::windows::wasapi
