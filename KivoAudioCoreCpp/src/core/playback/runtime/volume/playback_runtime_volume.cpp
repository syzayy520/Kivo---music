#include "../result/playback_runtime_result_factory.hpp"
#include "../state/playback_runtime_coordinator_state.hpp"

#include <algorithm>
#include <cmath>

namespace kivo::core::playback {

// Runtime software-volume command. State-neutral: it adjusts the active
// producer's processing-chain gain without changing CoreState. Ordering,
// generation, and legal-state validation are delegated to the session
// controller (which has a SetVolume case accepting Ready/Playing/Paused).
PlaybackRuntimeResult PlaybackRuntimeCoordinator::Impl::set_volume(
    const PlaybackCommand& command,
    double gain) noexcept {
    std::scoped_lock lock{mutex_};
    if (closed_) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::RuntimeClosed);
    }
    if (!active_ || !producer_) {
        return reject_command(command);
    }

    // Apply to the live chain FIRST — this is the validity gate. A non-unity
    // gain while bit-perfect is required is refused here; we must NOT persist
    // such a configuration, or a later seek/recovery create_producer() rebuild
    // would fail.
    const double clamped =
        std::isfinite(gain) ? std::clamp(gain, 0.0, 1.0) : 1.0;
    if (!producer_->set_volume(clamped)) {
        return reject_command(command);
    }

    const auto command_result = session_.submit(command);
    if (!command_result.accepted()) {
        return runtime_result::rejected(
            PlaybackRuntimeFailure::CommandRejected,
            command_result);
    }

    // Persist so seek/recovery rebuild the chain with this gain.
    producer_configuration_.processing.volume.enabled = true;
    producer_configuration_.processing.volume.linear_gain = clamped;
    return runtime_result::succeeded(command_result);
}

} // namespace kivo::core::playback
