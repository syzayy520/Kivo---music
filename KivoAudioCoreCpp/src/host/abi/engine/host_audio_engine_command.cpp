#include "host/abi/engine/host_audio_engine.hpp"

#include "host/abi/mapping/host_result_mapping.hpp"

namespace kivo::host::abi {

namespace {

[[nodiscard]] core::contract::CommandKind command_kind(
    uint32_t value) noexcept {
    using Kind = core::contract::CommandKind;
    switch (value) {
    case KIVO_AUDIO_COMMAND_RESUME: return Kind::Resume;
    case KIVO_AUDIO_COMMAND_PAUSE: return Kind::Pause;
    case KIVO_AUDIO_COMMAND_STOP: return Kind::Stop;
    case KIVO_AUDIO_COMMAND_SEEK: return Kind::Seek;
    case KIVO_AUDIO_COMMAND_FLUSH: return Kind::Flush;
    case KIVO_AUDIO_COMMAND_CLOSE_SOURCE: return Kind::CloseSource;
    case KIVO_AUDIO_COMMAND_SHUTDOWN: return Kind::Shutdown;
    case KIVO_AUDIO_COMMAND_SET_VOLUME: return Kind::SetVolume;
    default: return Kind::Unknown;
    }
}

} // namespace

kivo_audio_result HostAudioEngine::submit(
    const kivo_audio_command_v1& command) noexcept {
    std::unique_lock lock{command_mutex_, std::try_to_lock};
    if (!lock.owns_lock()) {
        return KIVO_AUDIO_RESULT_BUSY;
    }
    const auto kind = command_kind(command.kind);
    if (command.command_id == 0u
        || kind == core::contract::CommandKind::Unknown
        || (kind != core::contract::CommandKind::Shutdown
            && command.session_generation == 0u)) {
        last_result_ = KIVO_AUDIO_RESULT_INVALID_ARGUMENT;
        return last_result_;
    }
    // SET_VOLUME carries its gain in requested_frame as Q40.24. Decode and
    // range-check it HERE so the core never receives an out-of-range gain; the
    // overload is invisible below the ABI (the core sees a plain double).
    double set_volume_gain = 0.0;
    if (kind == core::contract::CommandKind::SetVolume) {
        set_volume_gain =
            static_cast<double>(command.requested_frame)
            / static_cast<double>(KIVO_AUDIO_VOLUME_GAIN_FIXED_ONE);
        if (set_volume_gain > 1.0) {
            last_result_ = KIVO_AUDIO_RESULT_INVALID_ARGUMENT;
            return last_result_;
        }
    }
    const core::playback::PlaybackCommand core_command{
        {command.command_id},
        kind,
        {
            command.session_generation,
            core::contract::CommandOrigin::External
        },
        kind == core::contract::CommandKind::Seek
            ? command.requested_frame
            : core::contract::kInvalidSamplePosition
    };
    const auto runtime_result =
        kind == core::contract::CommandKind::Seek
            ? runtime_.seek(core_command)
            : kind == core::contract::CommandKind::SetVolume
                ? runtime_.set_volume(core_command, set_volume_gain)
                : runtime_.execute(core_command);
    const auto result = map_runtime_result(runtime_result);
    last_result_ = result;
    lock.unlock();
    if (result != KIVO_AUDIO_RESULT_OK) {
        emit(
            result,
            KIVO_AUDIO_DIAGNOSTIC_DOMAIN_LIFECYCLE,
            command.command_id);
    }
    return result;
}

} // namespace kivo::host::abi
