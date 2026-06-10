// =============================================================================
// KivoAudioCoreCpp — channel_layout_tests.cpp
// Tests for ChannelLayout contract type
// =============================================================================

#include "kivo/core/contract/format/descriptor/channel_layout.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void channel_layout_values_are_distinct() {
    ASSERT(static_cast<uint8_t>(ChannelLayout::Unknown) == 0);
    ASSERT(ChannelLayout::Mono != ChannelLayout::Stereo);
    ASSERT(ChannelLayout::Surround51 != ChannelLayout::Surround71);
}

static void unknown_count_is_zero() {
    ASSERT(channel_layout_count(ChannelLayout::Unknown) == 0);
}

static void mono_count_is_1() {
    ASSERT(channel_layout_count(ChannelLayout::Mono) == 1);
}

static void stereo_count_is_2() {
    ASSERT(channel_layout_count(ChannelLayout::Stereo) == 2);
}

static void surround51_count_is_6() {
    ASSERT(channel_layout_count(ChannelLayout::Surround51) == 6);
}

static void surround71_count_is_8() {
    ASSERT(channel_layout_count(ChannelLayout::Surround71) == 8);
}

static void mono_is_not_multi_channel() {
    ASSERT(!channel_layout_is_multi_channel(ChannelLayout::Mono));
}

static void stereo_is_multi_channel() {
    ASSERT(channel_layout_is_multi_channel(ChannelLayout::Stereo));
}

static void unknown_is_not_multi_channel() {
    ASSERT(!channel_layout_is_multi_channel(ChannelLayout::Unknown));
}

void run_channel_layout_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- ChannelLayout ---\n";
    runner.run("channel_layout_values_are_distinct", channel_layout_values_are_distinct);
    runner.run("unknown_count_is_zero", unknown_count_is_zero);
    runner.run("mono_count_is_1", mono_count_is_1);
    runner.run("stereo_count_is_2", stereo_count_is_2);
    runner.run("surround51_count_is_6", surround51_count_is_6);
    runner.run("surround71_count_is_8", surround71_count_is_8);
    runner.run("mono_is_not_multi_channel", mono_is_not_multi_channel);
    runner.run("stereo_is_multi_channel", stereo_is_multi_channel);
    runner.run("unknown_is_not_multi_channel", unknown_is_not_multi_channel);
    std::cout << "\n";
}
