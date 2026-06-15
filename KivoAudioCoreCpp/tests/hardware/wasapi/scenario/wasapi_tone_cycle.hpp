#pragma once

#include <cstddef>
#include <span>

#include "../fixture/tone_options.hpp"

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"
#include "kivo/platform/windows/wasapi/renderer/wasapi_renderer.hpp"
#include "kivo/platform/windows/wasapi/worker/wasapi_render_worker.hpp"

namespace kivo::hardware_test::wasapi {

struct ToneCycleTotals {
    core::contract::FrameCount submitted_frames{0};
    uint64_t wait_timeouts{0};
    bool mmcss_registered{true};
    bool mmcss_high_priority{true};
    core::contract::BufferId next_buffer_id{1};
};

[[nodiscard]] int run_wasapi_tone_cycle(
    platform::windows::wasapi::WasapiRenderer& renderer,
    platform::windows::wasapi::WasapiRenderWorker& worker,
    core::render::SpscAudioBlockQueue& queue,
    std::span<std::byte> pcm,
    const core::contract::RenderFormat& format,
    core::contract::FrameCount chunk_frames,
    core::contract::FrameCount total_frames,
    const ToneOptions& options,
    uint32_t cycle_index,
    ToneCycleTotals& totals);

} // namespace kivo::hardware_test::wasapi
