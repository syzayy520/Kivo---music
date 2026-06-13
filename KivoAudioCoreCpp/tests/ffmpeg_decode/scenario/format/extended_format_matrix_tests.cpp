#include "../../fixture/assertion/fixture_decode_assertion.hpp"
#include "../../fixture/harness/ffmpeg_decode_test_runner.hpp"

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

void decodes_alac_m4a_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"tone_alac.m4a",
         core::decode::AudioCodec::Alac,
         core::decode::MediaContainer::Mp4});
}

void decodes_vorbis_ogg_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"tone_vorbis.ogg",
         core::decode::AudioCodec::Vorbis,
         core::decode::MediaContainer::Ogg});
}

void decodes_opus_ogg_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"tone_opus.ogg",
         core::decode::AudioCodec::Opus,
         core::decode::MediaContainer::Ogg,
         48000,
         core::contract::ChannelLayout::Stereo,
         95000,
         97000,
         false});
}

} // namespace

void run_ffmpeg_extended_format_matrix_tests(
    FfmpegDecodeTestRunner& runner) {
    runner.run(
        "decodes_alac_m4a_to_requested_pcm",
        decodes_alac_m4a_to_requested_pcm);
    runner.run(
        "decodes_vorbis_ogg_to_requested_pcm",
        decodes_vorbis_ogg_to_requested_pcm);
    runner.run(
        "decodes_opus_ogg_to_requested_pcm",
        decodes_opus_ogg_to_requested_pcm);
}
