// =============================================================================
// Kivo Music Qt - audio_pump_worker.cpp
// =============================================================================

#include "audio_pump_worker.hpp"

#include <chrono>

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <windows.h>
#endif

namespace kivo::qt::audio_bridge {

namespace {
// Producing: pump again almost immediately to stay ahead of the render worker.
constexpr auto kEagerPace = std::chrono::milliseconds(2);
// Queue full / idle / busy: back off so an idle player costs almost no CPU. Far
// shorter than the queue's drain time, so production never falls behind.
constexpr auto kBackoffPace = std::chrono::milliseconds(12);
} // namespace

AudioPumpWorker::AudioPumpWorker(std::function<bool()> step)
    : step_(std::move(step)) {}

AudioPumpWorker::~AudioPumpWorker() {
    stop();
}

void AudioPumpWorker::start() {
    bool expected = false;
    if (!running_.compare_exchange_strong(expected, true)) {
        return;  // already running
    }
    thread_ = std::thread(&AudioPumpWorker::run, this);
}

void AudioPumpWorker::stop() {
    running_.store(false, std::memory_order_relaxed);
    if (thread_.joinable()) {
        thread_.join();
    }
}

void AudioPumpWorker::run() {
#ifdef _WIN32
    // Feeds real-time audio: lift above normal UI/worker threads so QML/render
    // work can't starve production. Stays BELOW the render worker's Pro Audio
    // MMCSS priority, which remains the only hard-real-time thread.
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
#endif
    while (running_.load(std::memory_order_relaxed)) {
        const bool eager = step_ ? step_() : false;
        std::this_thread::sleep_for(eager ? kEagerPace : kBackoffPace);
    }
}

} // namespace kivo::qt::audio_bridge
