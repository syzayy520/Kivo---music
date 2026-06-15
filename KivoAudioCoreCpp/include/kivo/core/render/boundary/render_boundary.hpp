#pragma once

#include "kivo/core/render/boundary/render_control_boundary.hpp"
#include "kivo/core/render/boundary/render_write_boundary.hpp"

namespace kivo::core::render {

class RenderBoundary : public RenderControlBoundary, public RenderWriteBoundary {
public:
    ~RenderBoundary() override = default;
};

} // namespace kivo::core::render
