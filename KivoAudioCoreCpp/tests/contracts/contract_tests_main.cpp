// =============================================================================
// KivoAudioCoreCpp — contract_tests_main.cpp
// Unified entry point for all contract tests
// =============================================================================

#include <cassert>
#include <iostream>
#include <string>
#include <type_traits>

// Contract headers
#include "kivo/core/contract/result.hpp"
#include "kivo/core/contract/generation_id.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/contract/audio_format_descriptor.hpp"
#include "kivo/core/contract/clock_domain.hpp"
#include "kivo/core/contract/seek_flush.hpp"

using namespace kivo::core::contract;

// =============================================================================
// Test infrastructure
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
// Result/StatusOr tests
// =============================================================================
void result_ok_value() {
    ASSERT(Result::Ok != Result::Error);
}

void result_error_value() {
    ASSERT(Result::Error != Result::Ok);
}

void statusor_ok_construction() {
    auto s = StatusOr<int>::Ok(42);
    ASSERT(s.is_ok());
    ASSERT(!s.is_error());
    ASSERT(s.result() == Result::Ok);
}

void statusor_error_construction() {
    auto s = StatusOr<int>::Error("fail");
    ASSERT(!s.is_ok());
    ASSERT(s.is_error());
    ASSERT(s.result() == Result::Error);
}

void statusor_ok_value_access() {
    auto s = StatusOr<int>::Ok(42);
    ASSERT(s.value() == 42);
}

void statusor_error_message_access() {
    auto s = StatusOr<int>::Error("something went wrong");
    ASSERT(s.error() == "something went wrong");
}

void statusor_string_value() {
    auto s = StatusOr<std::string>::Ok("hello");
    ASSERT(s.value() == "hello");
}

void statusor_equality() {
    auto a = StatusOr<int>::Ok(42);
    auto b = StatusOr<int>::Ok(42);
    auto c = StatusOr<int>::Error("fail");
    ASSERT(a == b);
    ASSERT(!(a == c));
}

// =============================================================================
// GenerationId tests
// =============================================================================
void gen_id_initial_value_is_zero() {
    auto g = GenerationId::initial();
    ASSERT(g.value() == 0);
}

void gen_id_next_increments() {
    auto g0 = GenerationId::initial();
    auto g1 = GenerationId::next(g0);
    ASSERT(g1.value() == 1);
}

void gen_id_next_chain() {
    auto g = GenerationId::initial();
    for (uint64_t i = 0; i < 100; ++i) {
        g = GenerationId::next(g);
    }
    ASSERT(g.value() == 100);
}

void gen_id_equality_reflexive() {
    auto g = GenerationId::initial();
    ASSERT(g == g);
}

void gen_id_equality_same_value() {
    auto a = GenerationId::next(GenerationId::initial());
    auto b = GenerationId::next(GenerationId::initial());
    ASSERT(a == b);
}

void gen_id_inequality_different_value() {
    auto a = GenerationId::initial();
    auto b = GenerationId::next(a);
    ASSERT(!(a == b));
}

void gen_id_ordering() {
    auto a = GenerationId::initial();
    auto b = GenerationId::next(a);
    ASSERT(a < b);
    ASSERT(b > a);
    ASSERT(a <= b);
    ASSERT(b >= a);
}

// =============================================================================
// SamplePosition/FrameCount tests
// =============================================================================
void sample_position_is_uint64() {
    static_assert(std::is_same_v<SamplePosition, uint64_t>);
}

void frame_count_is_uint64() {
    static_assert(std::is_same_v<FrameCount, uint64_t>);
}

void invalid_position_is_max() {
    ASSERT(kInvalidSamplePosition == UINT64_MAX);
}

void zero_frame_count_is_zero() {
    ASSERT(kZeroFrameCount == 0);
}

void sample_position_arithmetic() {
    SamplePosition pos = 100;
    pos += 50;
    ASSERT(pos == 150);
}

void frame_count_arithmetic() {
    FrameCount count = 1024;
    count -= 256;
    ASSERT(count == 768);
}

// =============================================================================
// AudioFormatDescriptor tests
// =============================================================================
void sample_format_values() {
    ASSERT(static_cast<uint8_t>(SampleFormat::Unknown) == 0);
    ASSERT(SampleFormat::Int16 != SampleFormat::Int24);
    ASSERT(SampleFormat::Float32 != SampleFormat::Float64);
}

void channel_layout_values() {
    ASSERT(static_cast<uint8_t>(ChannelLayout::Unknown) == 0);
    ASSERT(ChannelLayout::Mono != ChannelLayout::Stereo);
    ASSERT(ChannelLayout::Surround51 != ChannelLayout::Surround71);
}

void fmt_default_construction_is_invalid() {
    AudioFormatDescriptor fmt;
    ASSERT(!fmt.is_valid());
}

void fmt_valid_format() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 44100;
    fmt.bits_per_sample = 16;
    ASSERT(fmt.is_valid());
}

void fmt_invalid_zero_sample_rate() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 0;
    fmt.bits_per_sample = 16;
    ASSERT(!fmt.is_valid());
}

void fmt_invalid_zero_bits() {
    AudioFormatDescriptor fmt;
    fmt.sample_format = SampleFormat::Int16;
    fmt.channel_layout = ChannelLayout::Stereo;
    fmt.sample_rate = 44100;
    fmt.bits_per_sample = 0;
    ASSERT(!fmt.is_valid());
}

void fmt_equality_same() {
    AudioFormatDescriptor a;
    a.sample_format = SampleFormat::Float32;
    a.channel_layout = ChannelLayout::Stereo;
    a.sample_rate = 48000;
    a.bits_per_sample = 32;
    AudioFormatDescriptor b = a;
    ASSERT(a == b);
}

void fmt_equality_different() {
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
// ClockDomain/ClockSnapshot tests
// =============================================================================
void clock_domain_values() {
    ASSERT(static_cast<uint8_t>(ClockDomain::Unknown) == 0);
    ASSERT(ClockDomain::Device != ClockDomain::Stream);
    ASSERT(ClockDomain::Stream != ClockDomain::Timeline);
}

void snap_default_is_invalid() {
    ClockSnapshot snap;
    ASSERT(!snap.is_valid());
}

void snap_valid() {
    ClockSnapshot snap;
    snap.domain = ClockDomain::Stream;
    snap.position = 44100;
    snap.generation = GenerationId::initial();
    ASSERT(snap.is_valid());
}

void snap_invalid_unknown_domain() {
    ClockSnapshot snap;
    snap.domain = ClockDomain::Unknown;
    snap.position = 100;
    ASSERT(!snap.is_valid());
}

void snap_invalid_position() {
    ClockSnapshot snap;
    snap.domain = ClockDomain::Device;
    snap.position = kInvalidSamplePosition;
    ASSERT(!snap.is_valid());
}

void snap_equality_same() {
    ClockSnapshot a;
    a.domain = ClockDomain::Timeline;
    a.position = 48000;
    a.generation = GenerationId::next(GenerationId::initial());
    ClockSnapshot b = a;
    ASSERT(a == b);
}

void snap_equality_different() {
    ClockSnapshot a;
    a.domain = ClockDomain::Stream;
    a.position = 100;
    a.generation = GenerationId::initial();

    ClockSnapshot b;
    b.domain = ClockDomain::Stream;
    b.position = 200;
    b.generation = GenerationId::initial();
    ASSERT(!(a == b));
}

// =============================================================================
// SeekAccuracy/FlushScope/DrainSemantics tests
// =============================================================================
void seek_accuracy_values() {
    ASSERT(SeekAccuracy::Exact != SeekAccuracy::NearestKeyframe);
    ASSERT(SeekAccuracy::NearestKeyframe != SeekAccuracy::NearestSample);
}

void flush_scope_none_is_zero() {
    ASSERT(static_cast<uint8_t>(FlushScope::None) == 0);
}

void flush_scope_all_is_or() {
    auto all = FlushScope::Decoder | FlushScope::Queue | FlushScope::Renderer;
    ASSERT(all == FlushScope::All);
}

void flush_scope_bitwise_or() {
    auto result = FlushScope::Decoder | FlushScope::Queue;
    ASSERT(has_flag(result, FlushScope::Decoder));
    ASSERT(has_flag(result, FlushScope::Queue));
    ASSERT(!has_flag(result, FlushScope::Renderer));
}

void flush_scope_has_flag() {
    ASSERT(has_flag(FlushScope::All, FlushScope::Decoder));
    ASSERT(has_flag(FlushScope::All, FlushScope::Queue));
    ASSERT(has_flag(FlushScope::All, FlushScope::Renderer));
    ASSERT(!has_flag(FlushScope::None, FlushScope::Decoder));
}

void flush_scope_bitwise_and() {
    auto result = FlushScope::All & FlushScope::Decoder;
    ASSERT(result == FlushScope::Decoder);
}

void drain_semantics_values() {
    ASSERT(DrainSemantics::None != DrainSemantics::DrainToZero);
    ASSERT(DrainSemantics::DrainToZero != DrainSemantics::DrainToSilence);
    ASSERT(DrainSemantics::DrainToSilence != DrainSemantics::Immediate);
}

// =============================================================================
// Main
// =============================================================================
int main() {
    std::cout << "=== Contract Tests ===\n\n";

    std::cout << "--- Result/StatusOr ---\n";
    TEST(result_ok_value);
    TEST(result_error_value);
    TEST(statusor_ok_construction);
    TEST(statusor_error_construction);
    TEST(statusor_ok_value_access);
    TEST(statusor_error_message_access);
    TEST(statusor_string_value);
    TEST(statusor_equality);

    std::cout << "\n--- GenerationId ---\n";
    TEST(gen_id_initial_value_is_zero);
    TEST(gen_id_next_increments);
    TEST(gen_id_next_chain);
    TEST(gen_id_equality_reflexive);
    TEST(gen_id_equality_same_value);
    TEST(gen_id_inequality_different_value);
    TEST(gen_id_ordering);

    std::cout << "\n--- SamplePosition/FrameCount ---\n";
    TEST(sample_position_is_uint64);
    TEST(frame_count_is_uint64);
    TEST(invalid_position_is_max);
    TEST(zero_frame_count_is_zero);
    TEST(sample_position_arithmetic);
    TEST(frame_count_arithmetic);

    std::cout << "\n--- AudioFormatDescriptor ---\n";
    TEST(sample_format_values);
    TEST(channel_layout_values);
    TEST(fmt_default_construction_is_invalid);
    TEST(fmt_valid_format);
    TEST(fmt_invalid_zero_sample_rate);
    TEST(fmt_invalid_zero_bits);
    TEST(fmt_equality_same);
    TEST(fmt_equality_different);

    std::cout << "\n--- ClockDomain/ClockSnapshot ---\n";
    TEST(clock_domain_values);
    TEST(snap_default_is_invalid);
    TEST(snap_valid);
    TEST(snap_invalid_unknown_domain);
    TEST(snap_invalid_position);
    TEST(snap_equality_same);
    TEST(snap_equality_different);

    std::cout << "\n--- SeekAccuracy/FlushScope/DrainSemantics ---\n";
    TEST(seek_accuracy_values);
    TEST(flush_scope_none_is_zero);
    TEST(flush_scope_all_is_or);
    TEST(flush_scope_bitwise_or);
    TEST(flush_scope_has_flag);
    TEST(flush_scope_bitwise_and);
    TEST(drain_semantics_values);

    std::cout << "\n=== " << tests_passed << "/" << tests_run << " passed ===\n";
    return (tests_passed == tests_run) ? 0 : 1;
}
