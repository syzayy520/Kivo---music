// =============================================================================
// Kivo Music Qt - audio_pump_worker.hpp
// 职责: 在专用线程上驱动引擎解码/产出泵（pump），把音频产出与 UI 线程调度解耦，
//       避免 QML 卡顿饿死 WASAPI 渲染线程而产生爆音/顿挫。
// 不变量: 本线程是引擎 SPSC 队列的【唯一生产者】——step 回调必须是 pump 的唯一调用方。
// =============================================================================

#pragma once

#include <atomic>
#include <functional>
#include <thread>

namespace kivo::qt::audio_bridge {

// Runs the engine producer pump on a dedicated thread, paced adaptively: tight
// while it is producing (stay ahead of the render worker), relaxed when the queue
// is full or playback is idle (negligible CPU). This is the SINGLE producer of the
// engine's single-producer/single-consumer audio block queue.
class AudioPumpWorker final {
public:
    // `step` performs ONE pump and returns true if it should pump again promptly
    // (produced frames and the queue is not full), false to back off (queue full,
    // idle, paused, or the engine was busy with a command).
    explicit AudioPumpWorker(std::function<bool()> step);
    ~AudioPumpWorker();

    AudioPumpWorker(const AudioPumpWorker&) = delete;
    AudioPumpWorker& operator=(const AudioPumpWorker&) = delete;

    void start();
    void stop();  // idempotent; signals the loop and joins the thread

private:
    void run();

    std::function<bool()> step_;
    std::thread thread_;
    std::atomic<bool> running_{false};
};

} // namespace kivo::qt::audio_bridge
