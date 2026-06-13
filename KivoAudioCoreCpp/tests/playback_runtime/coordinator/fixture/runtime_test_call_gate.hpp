#pragma once

#include <condition_variable>
#include <mutex>

namespace playback_runtime_test {

class RuntimeTestCallGate final {
public:
    void enter_and_wait() noexcept {
        std::unique_lock lock{mutex_};
        entered_ = true;
        condition_.notify_all();
        condition_.wait(lock, [this] { return released_; });
    }

    void wait_until_entered() {
        std::unique_lock lock{mutex_};
        condition_.wait(lock, [this] { return entered_; });
    }

    void release() noexcept {
        {
            std::scoped_lock lock{mutex_};
            released_ = true;
        }
        condition_.notify_all();
    }

private:
    std::mutex mutex_{};
    std::condition_variable condition_{};
    bool entered_{false};
    bool released_{false};
};

} // namespace playback_runtime_test
