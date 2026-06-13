#pragma once

#include "kivo/core/decode/failure/decode_failure.hpp"
#include "kivo/core/decode/source/byte_source_boundary.hpp"

namespace kivo::adapters::ffmpeg::detail {

[[nodiscard]] core::decode::DecodeFailure validate_known_container_extent(
    core::decode::ByteSourceBoundary& source) noexcept;

} // namespace kivo::adapters::ffmpeg::detail
