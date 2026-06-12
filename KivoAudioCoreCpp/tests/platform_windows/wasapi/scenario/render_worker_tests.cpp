#include "../fixture/wasapi_test_runner.hpp"

#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"
#include "kivo/platform/windows/wasapi/worker/wasapi_render_worker.hpp"

namespace {

using namespace kivo;

void closed_renderer_rejects_worker_start_without_side_effects() {
    platform::windows::wasapi::WasapiRenderer renderer;
    auto queue = core::render::SpscAudioBlockQueue::create({2, 64});
    WASAPI_ASSERT(queue);
    platform::windows::wasapi::WasapiRenderWorker worker(renderer, *queue);

    WASAPI_ASSERT(!worker.start());
    const auto snapshot = worker.snapshot();
    WASAPI_ASSERT(
        snapshot.state
        == platform::windows::wasapi::WasapiRenderWorkerState::Idle);
    WASAPI_ASSERT(snapshot.accepted_frames == 0);
    WASAPI_ASSERT(!snapshot.mmcss_registered);
}

void stop_and_join_are_idempotent_before_start() {
    platform::windows::wasapi::WasapiRenderer renderer;
    auto queue = core::render::SpscAudioBlockQueue::create({2, 64});
    WASAPI_ASSERT(queue);
    platform::windows::wasapi::WasapiRenderWorker worker(renderer, *queue);

    worker.request_stop();
    WASAPI_ASSERT(worker.join(std::chrono::milliseconds{0}));
    WASAPI_ASSERT(worker.join(std::chrono::milliseconds{0}));
}

} // namespace

void run_wasapi_render_worker_tests(WasapiTestRunner& runner) {
    runner.run(
        "closed_renderer_rejects_worker_start_without_side_effects",
        closed_renderer_rejects_worker_start_without_side_effects);
    runner.run(
        "stop_and_join_are_idempotent_before_start",
        stop_and_join_are_idempotent_before_start);
}
