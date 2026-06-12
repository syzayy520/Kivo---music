#pragma once

#include <cstdint>
#include <filesystem>

#include "kivo/core/decode/failure/decode_failure.hpp"

namespace ffmpeg_decode_test {

struct AbnormalMediaOutcome final {
    bool opened{false};
    bool reached_end_of_stream{false};
    kivo::core::decode::DecodeFailure failure{
        kivo::core::decode::DecodeFailure::None};
};

[[nodiscard]] AbnormalMediaOutcome decode_abnormal_media(
    const std::filesystem::path& path,
    uint64_t source_identity);

} // namespace ffmpeg_decode_test
