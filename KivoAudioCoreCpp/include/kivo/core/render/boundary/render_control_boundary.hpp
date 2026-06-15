#pragma once

#include "kivo/core/render/capability/renderer_capability_snapshot.hpp"
#include "kivo/core/render/format/render_open_request.hpp"
#include "kivo/core/render/format/render_open_result.hpp"
#include "kivo/core/render/identity/renderer_identity.hpp"
#include "kivo/core/render/lifecycle/render_control_request.hpp"
#include "kivo/core/render/lifecycle/render_control_result.hpp"
#include "kivo/core/render/snapshot/render_snapshot.hpp"

namespace kivo::core::render {

class RenderControlBoundary {
public:
    virtual ~RenderControlBoundary() = default;

    [[nodiscard]] virtual RendererIdentity identity() const noexcept = 0;
    [[nodiscard]] virtual RendererCapabilitySnapshot capabilities() const noexcept = 0;
    [[nodiscard]] virtual RenderOpenResult open(const RenderOpenRequest& request) noexcept = 0;
    [[nodiscard]] virtual RenderControlResult start() noexcept = 0;
    [[nodiscard]] virtual RenderControlResult flush(const RenderFlushRequest& request) noexcept = 0;
    [[nodiscard]] virtual RenderControlResult drain(const RenderDrainRequest& request) noexcept = 0;
    [[nodiscard]] virtual RenderControlResult reset(const RenderResetRequest& request) noexcept = 0;
    [[nodiscard]] virtual RenderControlResult stop() noexcept = 0;
    [[nodiscard]] virtual RenderControlResult close() noexcept = 0;
    [[nodiscard]] virtual RenderSnapshot snapshot() const noexcept = 0;
};

} // namespace kivo::core::render
