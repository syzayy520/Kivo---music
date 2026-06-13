#include "../../fixture/assertion/fixture_decode_assertion.hpp"
#include "../../fixture/harness/ffmpeg_decode_test_runner.hpp"

namespace {

using namespace kivo;
using namespace ffmpeg_decode_test;

void decodes_wave_to_requested_pcm(const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"tone_pcm_s16le.wav",
         core::decode::AudioCodec::Pcm,
         core::decode::MediaContainer::Wave});
}

void decodes_flac_to_requested_pcm(const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"tone.flac",
         core::decode::AudioCodec::Flac,
         core::decode::MediaContainer::Flac});
}

void decodes_mp3_to_requested_pcm(const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"tone.mp3",
         core::decode::AudioCodec::Mp3,
         core::decode::MediaContainer::Mp3,
         44100,
         core::contract::ChannelLayout::Stereo,
         95000,
         97000,
         false});
}

void decodes_vbr_mp3_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"tone_vbr.mp3",
         core::decode::AudioCodec::Mp3,
         core::decode::MediaContainer::Mp3,
         44100,
         core::contract::ChannelLayout::Stereo,
         95000,
         97000,
         false});
}

void decodes_aac_m4a_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"tone_aac.m4a",
         core::decode::AudioCodec::Aac,
         core::decode::MediaContainer::Mp4,
         44100,
         core::contract::ChannelLayout::Stereo,
         94000,
         97000});
}

void decodes_mono_22050_wave_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"mono_22050.wav",
         core::decode::AudioCodec::Pcm,
         core::decode::MediaContainer::Wave,
         22050,
         core::contract::ChannelLayout::Mono});
}

void decodes_stereo_96000_flac_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"stereo_96000.flac",
         core::decode::AudioCodec::Flac,
         core::decode::MediaContainer::Flac,
         96000});
}

void decodes_surround_48000_wave_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"surround_48000.wav",
         core::decode::AudioCodec::Pcm,
         core::decode::MediaContainer::Wave,
         48000,
         core::contract::ChannelLayout::Surround51,
         95000,
         97000,
         false});
}

void decodes_long_wave_to_requested_pcm(
    const FfmpegDecodeTestContext& context) {
    assert_fixture_decodes(
        context,
        {"long_pcm_s16le.wav",
         core::decode::AudioCodec::Pcm,
         core::decode::MediaContainer::Wave,
         44100,
         core::contract::ChannelLayout::Stereo,
         1439000,
         1441000});
}

} // namespace

void run_ffmpeg_baseline_decode_tests(FfmpegDecodeTestRunner& runner) {
    runner.run("decodes_wave_to_requested_pcm", decodes_wave_to_requested_pcm);
    runner.run("decodes_flac_to_requested_pcm", decodes_flac_to_requested_pcm);
    runner.run("decodes_mp3_to_requested_pcm", decodes_mp3_to_requested_pcm);
    runner.run(
        "decodes_vbr_mp3_to_requested_pcm",
        decodes_vbr_mp3_to_requested_pcm);
    runner.run(
        "decodes_aac_m4a_to_requested_pcm",
        decodes_aac_m4a_to_requested_pcm);
    runner.run(
        "decodes_mono_22050_wave_to_requested_pcm",
        decodes_mono_22050_wave_to_requested_pcm);
    runner.run(
        "decodes_stereo_96000_flac_to_requested_pcm",
        decodes_stereo_96000_flac_to_requested_pcm);
    runner.run(
        "decodes_surround_48000_wave_to_requested_pcm",
        decodes_surround_48000_wave_to_requested_pcm);
    runner.run(
        "decodes_long_wave_to_requested_pcm",
        decodes_long_wave_to_requested_pcm);
}
