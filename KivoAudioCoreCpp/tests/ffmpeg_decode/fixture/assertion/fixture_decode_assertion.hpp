#pragma once

#include "../decode/ffmpeg_decode_test_factory.hpp"

struct FfmpegDecodeTestContext;

namespace ffmpeg_decode_test {

void assert_fixture_decodes(
    const FfmpegDecodeTestContext& context,
    const FixtureExpectation& expectation);

} // namespace ffmpeg_decode_test
