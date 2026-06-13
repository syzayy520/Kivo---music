#include "host/abi/handle/host_handle_registry.hpp"

#include <limits>

namespace kivo::host::abi {

HostHandleRegistry& HostHandleRegistry::instance() noexcept {
    static HostHandleRegistry registry;
    return registry;
}

uint64_t HostHandleRegistry::insert(
    std::shared_ptr<HostAudioEngine> engine) {
    std::scoped_lock lock{mutex_};
    if (!engine) {
        return 0u;
    }
    while (next_handle_ == 0u
        || engines_.contains(next_handle_)) {
        ++next_handle_;
    }
    const auto handle = next_handle_;
    engines_.emplace(handle, std::move(engine));
    next_handle_ =
        handle == std::numeric_limits<uint64_t>::max()
            ? 1u
            : handle + 1u;
    return handle;
}

std::shared_ptr<HostAudioEngine> HostHandleRegistry::acquire(
    uint64_t handle) noexcept {
    std::scoped_lock lock{mutex_};
    const auto found = engines_.find(handle);
    return found == engines_.end()
        ? std::shared_ptr<HostAudioEngine>{}
        : found->second;
}

std::shared_ptr<HostAudioEngine> HostHandleRegistry::remove(
    uint64_t handle) noexcept {
    std::scoped_lock lock{mutex_};
    const auto found = engines_.find(handle);
    if (found == engines_.end()) {
        return {};
    }
    auto engine = std::move(found->second);
    engines_.erase(found);
    return engine;
}

} // namespace kivo::host::abi
