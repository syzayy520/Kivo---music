#pragma once

#include "kivo/core/render/boundary/render_write_boundary.hpp"
#include "kivo/core/render/pump/render_pump_result.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"

namespace kivo::core::render {

class RenderPump final {
public:
    RenderPump(
        SpscAudioBlockQueue& queue,
        RenderWriteBoundary& boundary) noexcept;

    [[nodiscard]] RenderPumpResult step() noexcept;

private:
    SpscAudioBlockQueue& queue_;
    RenderWriteBoundary& boundary_;
};

} // namespace kivo::core::render
