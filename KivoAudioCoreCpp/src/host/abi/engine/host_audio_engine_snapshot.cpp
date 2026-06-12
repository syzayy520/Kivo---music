#include "host/abi/engine/host_audio_engine.hpp"

#include "host/abi/mapping/host_result_mapping.hpp"

namespace kivo::host::abi {

kivo_audio_result HostAudioEngine::snapshot(
    kivo_audio_snapshot_v1& snapshot) noexcept {
    std::scoped_lock lock{command_mutex_};
    const auto current = runtime_.snapshot();
    snapshot = {
        sizeof(kivo_audio_snapshot_v1),
        KIVO_AUDIO_ABI_STRUCT_VERSION_1,
        static_cast<uint32_t>(map_core_state(current.session.state)),
        current.active ? 1u : 0u,
        current.session.session_generation,
        current.session.last_command_id.value,
        current.session.rendered_position,
        current.producer.decoded_frames,
        current.queue.queued_frames,
        current.successful_opens,
        current.successful_seeks,
        current.failed_operations,
        current.stale_blocks_rejected,
        current.device_loss_events,
        current.decode_failure_events,
        last_result_,
        0u
    };
    return KIVO_AUDIO_RESULT_OK;
}

} // namespace kivo::host::abi
