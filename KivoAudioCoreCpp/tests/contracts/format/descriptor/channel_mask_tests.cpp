// =============================================================================
// KivoAudioCoreCpp — channel_mask_tests.cpp
// Tests for ChannelMask and ChannelMaskBit contract types
// =============================================================================

#include "kivo/core/contract/format/descriptor/channel_mask.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_mask_is_zero() {
    ChannelMask mask;
    ASSERT(mask.value == 0);
}

static void mask_has_bit() {
    ChannelMask mask;
    mask.value = static_cast<uint32_t>(ChannelMaskBit::FrontLeft)
               | static_cast<uint32_t>(ChannelMaskBit::FrontRight);
    ASSERT(mask.has(ChannelMaskBit::FrontLeft));
    ASSERT(mask.has(ChannelMaskBit::FrontRight));
    ASSERT(!mask.has(ChannelMaskBit::FrontCenter));
}

static void stereo_mask_is_stereo() {
    ASSERT(kStereoMask.is_stereo());
}

static void mono_mask_is_not_stereo() {
    ASSERT(!kMonoMask.is_stereo());
}

static void stereo_mask_equality() {
    ChannelMask a = kStereoMask;
    ChannelMask b = kStereoMask;
    ASSERT(a == b);
}

static void different_masks_not_equal() {
    ASSERT(!(kMonoMask == kStereoMask));
}

static void surround51_mask_has_lfe() {
    ASSERT(kSurround51Mask.has(ChannelMaskBit::LowFrequency));
}

static void surround71_mask_has_sides() {
    ASSERT(kSurround71Mask.has(ChannelMaskBit::SideLeft));
    ASSERT(kSurround71Mask.has(ChannelMaskBit::SideRight));
}

void run_channel_mask_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- ChannelMask ---\n";
    runner.run("default_mask_is_zero", default_mask_is_zero);
    runner.run("mask_has_bit", mask_has_bit);
    runner.run("stereo_mask_is_stereo", stereo_mask_is_stereo);
    runner.run("mono_mask_is_not_stereo", mono_mask_is_not_stereo);
    runner.run("stereo_mask_equality", stereo_mask_equality);
    runner.run("different_masks_not_equal", different_masks_not_equal);
    runner.run("surround51_mask_has_lfe", surround51_mask_has_lfe);
    runner.run("surround71_mask_has_sides", surround71_mask_has_sides);
    std::cout << "\n";
}
