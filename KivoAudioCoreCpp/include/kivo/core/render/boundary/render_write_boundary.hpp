#pragma once

#include "kivo/core/render/buffer/audio_block_view.hpp"
#include "kivo/core/render/write/render_write_result.hpp"

namespace kivo::core::render {

class RenderWriteBoundary {
public:
    virtual ~RenderWriteBoundary() = default;

    [[nodiscard]] virtual RenderWriteResult write(const AudioBlockView& block) noexcept = 0;
};

} // namespace kivo::core::render
