// =============================================================================
// KivoAudioCoreCpp — audio_format_descriptor_tests.cpp
// Tests for AudioFormatDescriptor contract type
// =============================================================================

#include "kivo/core/contract/format/descriptor/audio_format_descriptor.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_construction_is_invalid() {
    AudioFormatDescriptor fmt;
    ASSERT(!fmt.is_valid());
}

static void valid_format() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 44100;
    fmt.bits_per_sample = 16;
    ASSERT(fmt.is_valid());
}

static void invalid_zero_sample_rate() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 0;
    fmt.bits_per_sample = 16;
    ASSERT(!fmt.is_valid());
}

static void invalid_zero_bits() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 44100;
    fmt.bits_per_sample = 0;
    ASSERT(!fmt.is_valid());
}

static void invalid_mismatched_valid_bits() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 44100;
    fmt.bits_per_sample = 24;
    ASSERT(!fmt.is_valid());
}

static void invalid_mismatched_container_bits() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int24;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 48000;
    fmt.bits_per_sample = 24;
    fmt.container_bits_per_sample = 24;
    ASSERT(!fmt.is_valid());
}

static void invalid_unknown_frame_layout() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.frame_layout = FrameLayout::Unknown;
    fmt.sample_rate = 44100;
    fmt.bits_per_sample = 16;
    ASSERT(!fmt.is_valid());
}

static void invalid_channel_mask_mismatch() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.channel_mask = kMonoMask;
    fmt.sample_rate = 44100;
    fmt.bits_per_sample = 16;
    ASSERT(!fmt.is_valid());
}

static void channel_count_derived() {
    AudioFormatDescriptor fmt;
    fmt.channel_layout = ChannelLayout::Stereo;
    ASSERT(fmt.channel_count() == 2);

    fmt.channel_layout = ChannelLayout::Surround51;
    ASSERT(fmt.channel_count() == 6);
}

static void bytes_per_frame_calculation() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.bits_per_sample = 16;
    ASSERT(fmt.bytes_per_frame() == 4); // 2 bytes/sample * 2 channels
}

static void int24_stereo_uses_32_bit_container() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int24;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 48000;
    fmt.bits_per_sample = 24;
    ASSERT(fmt.is_valid());
    ASSERT(fmt.valid_bits_per_sample() == 24);
    ASSERT(fmt.container_bits() == 32);
    ASSERT(fmt.bytes_per_frame() == 8);
}

static void explicit_container_bits_normalize_equality() {
    AudioFormatDescriptor a;
    a.sample_format = SampleFormat::Int24;
    a.channel_layout = ChannelLayout::Stereo;
    a.sample_rate = 48000;
    a.bits_per_sample = 24;

    AudioFormatDescriptor b = a;
    b.container_bits_per_sample = 32;

    ASSERT(a == b);
}

static void implicit_and_explicit_channel_masks_normalize_equality() {
    AudioFormatDescriptor a;
    a.sample_format = SampleFormat::Float32;
    a.channel_layout = ChannelLayout::Surround51;
    a.sample_rate = 48000;
    a.bits_per_sample = 32;

    AudioFormatDescriptor b = a;
    b.channel_mask = kSurround51Mask;

    ASSERT(a.is_valid());
    ASSERT(b.is_valid());
    ASSERT(a == b);
    ASSERT(a.channel_count() == 6);
    ASSERT(a.bytes_per_frame() == 24);
}

static void common_layouts_have_consistent_masks_and_frame_sizes() {
    AudioFormatDescriptor mono{
        SampleFormat::Int16,
        ChannelLayout::Mono,
        FrameLayout::Interleaved,
        {},
        44100,
        16,
        0
    };
    AudioFormatDescriptor surround71{
        SampleFormat::Float32,
        ChannelLayout::Surround71,
        FrameLayout::Planar,
        {},
        96000,
        32,
        0
    };

    ASSERT(mono.is_valid());
    ASSERT(mono.effective_channel_mask() == kMonoMask);
    ASSERT(mono.bytes_per_frame() == 2);
    ASSERT(surround71.is_valid());
    ASSERT(surround71.effective_channel_mask() == kSurround71Mask);
    ASSERT(surround71.channel_count() == 8);
    ASSERT(surround71.bytes_per_frame() == 32);
}

static void equality_same() {
    AudioFormatDescriptor a;
    a.sample_format = SampleFormat::Float32;
    a.channel_layout = ChannelLayout::Stereo;
    a.sample_rate = 48000;
    a.bits_per_sample = 32;

    AudioFormatDescriptor b = a;
    ASSERT(a == b);
}

static void equality_different() {
    AudioFormatDescriptor a;
    a.sample_format = SampleFormat::Int16;
    a.channel_layout = ChannelLayout::Stereo;
    a.sample_rate = 44100;
    a.bits_per_sample = 16;

    AudioFormatDescriptor b;
    b.sample_format = SampleFormat::Int16;
    b.channel_layout = ChannelLayout::Stereo;
    b.sample_rate = 48000;
    b.bits_per_sample = 16;

    ASSERT(!(a == b));
}

void run_audio_format_descriptor_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- AudioFormatDescriptor ---\n";
    runner.run("default_construction_is_invalid", default_construction_is_invalid);
    runner.run("valid_format", valid_format);
    runner.run("invalid_zero_sample_rate", invalid_zero_sample_rate);
    runner.run("invalid_zero_bits", invalid_zero_bits);
    runner.run("invalid_mismatched_valid_bits", invalid_mismatched_valid_bits);
    runner.run("invalid_mismatched_container_bits", invalid_mismatched_container_bits);
    runner.run("invalid_unknown_frame_layout", invalid_unknown_frame_layout);
    runner.run("invalid_channel_mask_mismatch", invalid_channel_mask_mismatch);
    runner.run("channel_count_derived", channel_count_derived);
    runner.run("bytes_per_frame_calculation", bytes_per_frame_calculation);
    runner.run("int24_stereo_uses_32_bit_container", int24_stereo_uses_32_bit_container);
    runner.run("explicit_container_bits_normalize_equality", explicit_container_bits_normalize_equality);
    runner.run("implicit_and_explicit_channel_masks_normalize_equality", implicit_and_explicit_channel_masks_normalize_equality);
    runner.run("common_layouts_have_consistent_masks_and_frame_sizes", common_layouts_have_consistent_masks_and_frame_sizes);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
