#pragma once

#include "kivo/core/decode/failure/decode_failure.hpp"
#include "kivo/core/decode/media/audio_codec.hpp"
#include "kivo/core/decode/media/media_container.hpp"
#include "kivo/core/decode/media/support/media_playback_support.hpp"

namespace kivo::core::decode {

struct MediaSupportClassification {
    AudioCodec codec{AudioCodec::Unknown};
    MediaContainer container{MediaContainer::Unknown};
    bool dop_carrier{false};
    MediaPlaybackSupport support{MediaPlaybackSupport::Unknown};
    DecodeFailure failure{DecodeFailure::UnsupportedCodec};

    [[nodiscard]] constexpr bool is_supported() const noexcept {
        return media_playback_is_supported(support)
            && failure == DecodeFailure::None;
    }

    [[nodiscard]] constexpr bool operator==(
        const MediaSupportClassification&) const noexcept = default;
};

[[nodiscard]] constexpr bool media_container_is_known(
    MediaContainer container) noexcept {
    return container != MediaContainer::Unknown;
}

[[nodiscard]] constexpr bool audio_codec_is_known(
    AudioCodec codec) noexcept {
    return codec != AudioCodec::Unknown;
}

[[nodiscard]] constexpr bool media_container_is_dsd(
    MediaContainer container) noexcept {
    return container == MediaContainer::Dsf
        || container == MediaContainer::Dsdiff;
}

[[nodiscard]] constexpr MediaSupportClassification classify_media_support(
    AudioCodec codec,
    MediaContainer container,
    bool dop_carrier = false) noexcept {
    if (dop_carrier) {
        return {
            codec,
            container,
            true,
            MediaPlaybackSupport::UnsupportedDopCarrier,
            DecodeFailure::UnsupportedCodec
        };
    }
    if (!media_container_is_known(container)) {
        return {
            codec,
            container,
            false,
            MediaPlaybackSupport::UnsupportedContainer,
            DecodeFailure::UnsupportedContainer
        };
    }
    if (!audio_codec_is_known(codec)) {
        return {
            codec,
            container,
            false,
            MediaPlaybackSupport::UnsupportedCodec,
            DecodeFailure::UnsupportedCodec
        };
    }
    if (codec == AudioCodec::Dsd || media_container_is_dsd(container)) {
        return {
            codec,
            container,
            false,
            MediaPlaybackSupport::UnsupportedNativeDsd,
            DecodeFailure::UnsupportedCodec
        };
    }
    return {
        codec,
        container,
        false,
        MediaPlaybackSupport::SupportedPcm,
        DecodeFailure::None
    };
}

} // namespace kivo::core::decode
