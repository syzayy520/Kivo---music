#pragma once

#include "kivo/core/contract/cue/continuity/codec_delay.hpp"
#include "kivo/core/contract/cue/continuity/padding.hpp"
#include "kivo/core/contract/format/negotiation/conversion_policy.hpp"
#include "kivo/core/contract/format/negotiation/resample_decision.hpp"
#include "kivo/core/contract/format/roles/native_decoded_format.hpp"
#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/contract/source/identity/source_generation.hpp"
#include "kivo/core/contract/source/identity/source_identity.hpp"
#include "kivo/core/decode/generation/decode_generation.hpp"
#include "kivo/core/decode/media/audio_codec.hpp"
#include "kivo/core/decode/media/media_container.hpp"
#include "kivo/core/processing/snapshot/audio_conversion_snapshot.hpp"

namespace kivo::core::decode {

struct MediaProbe {
    contract::SourceIdentity source_identity{};
    contract::SourceGeneration source_generation{};
    DecodeGeneration decode_generation{};
    AudioCodec codec{AudioCodec::Unknown};
    MediaContainer container{MediaContainer::Unknown};
    contract::NativeDecodedFormat native_format{};
    contract::RenderFormat output_format{};
    contract::ConversionPolicy conversion{};
    contract::ResampleDecision resample{};
    processing::AudioConversionSnapshot conversion_snapshot{};
    contract::CodecDelay codec_delay{};
    contract::Padding trailing_padding{};
    contract::FrameCount duration_frames{0};
    bool duration_known{false};
    bool seekable{false};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return source_identity.value != 0
            && codec != AudioCodec::Unknown
            && container != MediaContainer::Unknown
            && native_format.is_valid()
            && output_format.is_valid()
            && (!duration_known || duration_frames != 0)
            && conversion_snapshot.conversion == conversion
            && resample.source_rate == native_format.format.sample_rate
            && resample.target_rate == output_format.format.sample_rate;
    }

    [[nodiscard]] constexpr bool operator==(
        const MediaProbe&) const noexcept = default;
};

} // namespace kivo::core::decode
