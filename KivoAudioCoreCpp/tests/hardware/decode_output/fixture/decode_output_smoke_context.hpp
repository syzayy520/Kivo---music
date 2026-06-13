#pragma once

#include <filesystem>
#include <memory>
#include <utility>

#include "kivo/adapters/ffmpeg/session/ffmpeg_audio_decode_session.hpp"
#include "kivo/core/decode/media/media_probe.hpp"
#include "kivo/core/playback/pipeline/producer/decode_render_queue_producer.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"
#include "kivo/platform/windows/wasapi/renderer/wasapi_renderer.hpp"
#include "kivo/platform/windows/wasapi/worker/wasapi_render_worker.hpp"

namespace kivo::hardware_test::decode_output {

struct DecodeOutputSmokeContext {
    explicit DecodeOutputSmokeContext(std::filesystem::path source_path)
        : path(std::move(source_path)) {}

    std::filesystem::path path;
    platform::windows::wasapi::WasapiRenderer renderer;
    adapters::ffmpeg::FfmpegAudioDecodeSession decoder;
    std::unique_ptr<core::render::SpscAudioBlockQueue> queue;
    std::unique_ptr<platform::windows::wasapi::WasapiRenderWorker> worker;
    std::unique_ptr<core::playback::DecodeRenderQueueProducer> producer;
    core::contract::RenderFormat format{};
    core::decode::MediaProbe probe{};
    core::contract::FrameCount render_buffer_frames{0};
    core::playback::DecodeRenderQueueProducerSnapshot producer_snapshot{};
    platform::windows::wasapi::WasapiRenderWorkerSnapshot worker_snapshot{};
    bool reached_eos{false};
};

} // namespace kivo::hardware_test::decode_output
