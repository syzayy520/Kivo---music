#include "../state/playback_runtime_coordinator_state.hpp"

namespace kivo::core::playback {

PlaybackRuntimeSnapshot
PlaybackRuntimeCoordinator::Impl::snapshot() const noexcept {
    std::scoped_lock lock{mutex_};
    PlaybackRuntimeSnapshot result{};
    result.active = active_;
    result.format = format_;
    result.decode_generation = decode_generation_;
    result.render_generations = generations_;
    result.session = session_.snapshot();
    result.renderer = renderer_.snapshot();
    if (queue_) {
        result.queue = queue_->snapshot();
    }
    if (producer_) {
        result.producer = producer_->snapshot();
    }
    result.successful_opens = successful_opens_;
    result.rolled_back_opens = rolled_back_opens_;
    result.successful_seeks = successful_seeks_;
    result.failed_operations = failed_operations_;
    result.stale_blocks_rejected = stale_blocks_rejected_;
    result.drain_attempts = drain_attempts_;
    result.successful_drains = successful_drains_;
    result.failed_drains = failed_drains_;
    result.drain_timeouts = drain_timeouts_;
    return result;
}

} // namespace kivo::core::playback
