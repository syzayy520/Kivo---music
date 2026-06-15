#include "../../fixture/decode/ffmpeg_decode_test_factory.hpp"
#include "../../fixture/harness/ffmpeg_decode_test_runner.hpp"

#include <filesystem>
#include <fstream>

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

void cue_sheet_input_is_explicitly_unsupported(
    const FfmpegDecodeTestContext& context) {
    const auto path = context.fixture_directory / "whole_album.cue";
    {
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        output
            << "FILE \"tone.flac\" WAVE\n"
            << "  TRACK 01 AUDIO\n"
            << "    INDEX 01 00:00:00\n";
        FFMPEG_ASSERT(output.good());
    }

    adapters::ffmpeg::FfmpegAudioDecodeSession session;
    auto source = open_source(path, 501);
    FFMPEG_ASSERT(source != nullptr);
    const auto opened = session.open(std::move(source), open_request());
    FFMPEG_ASSERT(!opened.is_accepted());
    FFMPEG_ASSERT(
        opened.failure()
        == core::decode::DecodeFailure::UnsupportedContainer);
    std::filesystem::remove(path);
}

} // namespace

void run_ffmpeg_unsupported_cue_tests(FfmpegDecodeTestRunner& runner) {
    runner.run(
        "cue_sheet_input_is_explicitly_unsupported",
        cue_sheet_input_is_explicitly_unsupported);
}
