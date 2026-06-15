#pragma once

#include <cstdint>

namespace kivo::core::decode {

enum class MediaPlaybackSupport : uint8_t {
    Unknown = 0,
    SupportedPcm,
    UnsupportedNativeDsd,
    UnsupportedDopCarrier,
    UnsupportedContainer,
    UnsupportedCodec
};

[[nodiscard]] constexpr bool media_playback_is_supported(
    MediaPlaybackSupport support) noexcept {
    return support == MediaPlaybackSupport::SupportedPcm;
}

} // namespace kivo::core::decode
