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
    runner.run("channel_count_derived", channel_count_derived);
    runner.run("bytes_per_frame_calculation", bytes_per_frame_calculation);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
