// =============================================================================
// KivoAudioCoreCpp — audio_format_descriptor_tests.cpp
// Tests for AudioFormatDescriptor contract type
// =============================================================================

#include "kivo/core/contract/audio_format_descriptor.hpp"
#include <cassert>
#include <iostream>

using namespace kivo::core::contract;

// =============================================================================
// Test helpers
// =============================================================================
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        tests_run++; \
        std::cout << "  " << #name << "... "; \
        try { \
            name(); \
            tests_passed++; \
            std::cout << "PASS\n"; \
        } catch (const std::exception& e) { \
            std::cout << "FAIL: " << e.what() << "\n"; \
        } \
    } while(0)

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error("Assertion failed: " #cond); \
        } \
    } while(0)

// =============================================================================
// SampleFormat enum tests
// =============================================================================
void sample_format_values() {
    ASSERT(static_cast<uint8_t>(SampleFormat::Unknown) == 0);
    ASSERT(SampleFormat::Int16 != SampleFormat::Int24);
    ASSERT(SampleFormat::Float32 != SampleFormat::Float64);
}

// =============================================================================
// ChannelLayout enum tests
// =============================================================================
void channel_layout_values() {
    ASSERT(static_cast<uint8_t>(ChannelLayout::Unknown) == 0);
    ASSERT(ChannelLayout::Mono != ChannelLayout::Stereo);
    ASSERT(ChannelLayout::Surround51 != ChannelLayout::Surround71);
}

// =============================================================================
// AudioFormatDescriptor tests
// =============================================================================
void default_construction_is_invalid() {
    AudioFormatDescriptor fmt;
    ASSERT(!fmt.is_valid());
}

void valid_format() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 44100;
    fmt.bits_per_sample = 16;
    ASSERT(fmt.is_valid());
}

void invalid_zero_sample_rate() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 0;
    fmt.bits_per_sample = 16;
    ASSERT(!fmt.is_valid());
}

void invalid_zero_bits() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 44100;
    fmt.bits_per_sample = 0;
    ASSERT(!fmt.is_valid());
}

void equality_same() {
    AudioFormatDescriptor a;
    a.sample_format = SampleFormat::Float32;
    a.channel_layout = ChannelLayout::Stereo;
    a.sample_rate = 48000;
    a.bits_per_sample = 32;

    AudioFormatDescriptor b = a;
    ASSERT(a == b);
}

void equality_different() {
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
// Main
// =============================================================================
int main() {
    std::cout << "audio_format_descriptor_tests:\n";

    TEST(sample_format_values);
    TEST(channel_layout_values);
    TEST(default_construction_is_invalid);
    TEST(valid_format);
    TEST(invalid_zero_sample_rate);
    TEST(invalid_zero_bits);
    TEST(equality_same);
    TEST(equality_different);

    std::cout << "\n  " << tests_passed << "/" << tests_run << " passed\n";
    return (tests_passed == tests_run) ? 0 : 1;
}
