#pragma once

#include <cstdint>

namespace kivo::core::playback {

enum class PlaybackRuntimeFailure : uint8_t {
    None = 0,
    InvalidRequest,
    UnsupportedCommand,
    NoActiveRuntime,
    CommandRejected,
    QueueCreationFailed,
    RenderOpenFailed,
    DecodeOpenFailed,
    ProducerCreationFailed,
    RenderControlFailed,
    DecodeControlFailed,
    RuntimeClosed,
    BoundaryFailure
};

} // namespace kivo::core::playback
