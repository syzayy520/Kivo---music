// =============================================================================
// KivoAudioCoreCpp — audio_format_descriptor_tests.cpp
// Tests for AudioFormatDescriptor contract type
// =============================================================================

#include "kivo/core/contract/format/audio_format_descriptor.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void sample_format_values() {
    ASSERT(static_cast<uint8_t>(SampleFormat::Unknown) == 0);
    ASSERT(SampleFormat::Int16 != SampleFormat::Int24);
    ASSERT(SampleFormat::Float32 != SampleFormat::Float64);
}

static void channel_layout_values() {
    ASSERT(static_cast<uint8_t>(ChannelLayout::Unknown) == 0);
    ASSERT(ChannelLayout::Mono != ChannelLayout::Stereo);
    ASSERT(ChannelLayout::Surround51 != ChannelLayout::Surround71);
}

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

// =============================================================================
// Runner
// =============================================================================
void run_audio_format_descriptor_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- AudioFormatDescriptor ---\n";
    runner.run("sample_format_values", sample_format_values);
    runner.run("channel_layout_values", channel_layout_values);
    runner.run("default_construction_is_invalid", default_construction_is_invalid);
    runner.run("valid_format", valid_format);
    runner.run("invalid_zero_sample_rate", invalid_zero_sample_rate);
    runner.run("invalid_zero_bits", invalid_zero_bits);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
