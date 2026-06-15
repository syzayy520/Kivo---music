#pragma once

#include <cstddef>

#include "kivo/core/contract/buffer/identity/buffer_id.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/playback/pipeline/configuration/queue_end_of_stream_policy.hpp"
#include "kivo/core/processing/configuration/audio_processing_configuration.hpp"

namespace kivo::core::playback {

struct DecodeRenderQueueProducerConfiguration {
    size_t maximum_decode_block_bytes{0};
    contract::FrameCount maximum_chunk_frames{0};
    contract::BufferId first_buffer_id{1};
    contract::SamplePosition timeline_origin_frames{0};
    QueueEndOfStreamPolicy end_of_stream_policy{
        QueueEndOfStreamPolicy::CloseAndMarkFinal};
    processing::AudioProcessingConfiguration processing{};

    [[nodiscard]] bool is_valid() const noexcept {
        return maximum_decode_block_bytes != 0
            && maximum_chunk_frames != 0
            && first_buffer_id.value != 0
            && timeline_origin_frames
                != contract::kInvalidSamplePosition
            && processing.is_valid();
    }

    [[nodiscard]] bool operator==(
        const DecodeRenderQueueProducerConfiguration&) const noexcept = default;
};

} // namespace kivo::core::playback
