#pragma once

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "host/abi/engine/host_audio_engine.hpp"

namespace kivo::host::abi {

class HostHandleRegistry final {
public:
    [[nodiscard]] static HostHandleRegistry& instance() noexcept;

    [[nodiscard]] uint64_t insert(
        std::shared_ptr<HostAudioEngine> engine);
    [[nodiscard]] std::shared_ptr<HostAudioEngine> acquire(
        uint64_t handle) noexcept;
    [[nodiscard]] std::shared_ptr<HostAudioEngine> remove(
        uint64_t handle) noexcept;

private:
    std::mutex mutex_;
    std::unordered_map<uint64_t, std::shared_ptr<HostAudioEngine>> engines_;
    uint64_t next_handle_{1};
};

} // namespace kivo::host::abi
