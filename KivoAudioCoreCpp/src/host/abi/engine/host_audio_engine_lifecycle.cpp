#include "host/abi/engine/host_audio_engine.hpp"

#include "host/abi/mapping/host_result_mapping.hpp"

namespace kivo::host::abi {

HostAudioEngine::HostAudioEngine(
    const kivo_audio_create_info_v1& create_info)
    : runtime_(session_, decoder_, renderer_),
      diagnostic_context_(create_info.diagnostic_context),
      diagnostic_callback_(create_info.diagnostic_callback) {}

HostAudioEngine::~HostAudioEngine() {
    shutdown_for_destroy();
}

uint64_t HostAudioEngine::next_internal_command_id() const noexcept {
    const auto last = session_.snapshot().last_command_id.value;
    return last == UINT64_MAX ? UINT64_MAX : last + 1u;
}

void HostAudioEngine::shutdown_for_destroy() noexcept {
    std::scoped_lock lock{command_mutex_};
    const auto current = runtime_.snapshot();
    if (current.session.state
        == core::contract::CoreState::Closed) {
        return;
    }
    const core::playback::PlaybackCommand command{
        {next_internal_command_id()},
        core::contract::CommandKind::Shutdown,
        {
            current.session.session_generation,
            core::contract::CommandOrigin::System
        },
        core::contract::kInvalidSamplePosition
    };
    static_cast<void>(runtime_.execute(command));
}

void HostAudioEngine::emit(
    kivo_audio_result result,
    kivo_audio_diagnostic_domain domain,
    uint64_t command_id) noexcept {
    if (diagnostic_callback_ == nullptr) {
        return;
    }
    const kivo_audio_diagnostic_event_v1 event{
        sizeof(kivo_audio_diagnostic_event_v1),
        KIVO_AUDIO_ABI_STRUCT_VERSION_1,
        ++diagnostic_sequence_,
        command_id,
        static_cast<uint32_t>(domain),
        static_cast<uint32_t>(result),
        result,
        0u
    };
    try {
        diagnostic_callback_(diagnostic_context_, &event);
    } catch (...) {
    }
}

kivo_audio_result HostAudioEngine::capabilities(
    kivo_audio_capabilities_v1& capabilities) noexcept {
    capabilities = {
        sizeof(kivo_audio_capabilities_v1),
        KIVO_AUDIO_ABI_STRUCT_VERSION_1,
        KIVO_AUDIO_ABI_VERSION_MAJOR,
        KIVO_AUDIO_ABI_VERSION_MINOR,
        KIVO_AUDIO_ABI_VERSION_PATCH,
        KIVO_AUDIO_ABI_STRUCT_VERSION_1,
        KIVO_AUDIO_CAPABILITY_HOST_SOURCE_CALLBACKS
            | KIVO_AUDIO_CAPABILITY_SEEK
            | KIVO_AUDIO_CAPABILITY_SHARED_OUTPUT
            | KIVO_AUDIO_CAPABILITY_EXCLUSIVE_OUTPUT
            | KIVO_AUDIO_CAPABILITY_REPLAY_GAIN
            | KIVO_AUDIO_CAPABILITY_SOFTWARE_VOLUME
            | KIVO_AUDIO_CAPABILITY_DITHER
            | KIVO_AUDIO_CAPABILITY_SANITIZED_DIAGNOSTICS
            | KIVO_AUDIO_CAPABILITY_EXTENDED_DIAGNOSTIC_SNAPSHOT,
        KIVO_AUDIO_THREAD_RULE_SERIALIZE_COMMANDS
            | KIVO_AUDIO_THREAD_RULE_SOURCE_ON_CALLER
            | KIVO_AUDIO_THREAD_RULE_DIAGNOSTIC_ON_CALLER
            | KIVO_AUDIO_THREAD_RULE_NO_RENDER_CALLBACKS,
        core::render::SpscAudioBlockQueueConfiguration::kMaximumSlotCount,
        384000u,
        8u,
        {0u, 0u, 0u, 0u}
    };
    return KIVO_AUDIO_RESULT_OK;
}

} // namespace kivo::host::abi
