// =============================================================================
// KivoAudioCoreCpp — frame_layout_tests.cpp
// Tests for FrameLayout contract type
// =============================================================================

#include "kivo/core/contract/format/descriptor/frame_layout.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void frame_layout_values_are_distinct() {
    ASSERT(static_cast<uint8_t>(FrameLayout::Unknown) == 0);
    ASSERT(FrameLayout::Interleaved != FrameLayout::Planar);
    ASSERT(FrameLayout::Interleaved != FrameLayout::Unknown);
    ASSERT(FrameLayout::Planar != FrameLayout::Unknown);
}

static void interleaved_is_not_planar() {
    ASSERT(FrameLayout::Interleaved != FrameLayout::Planar);
}

void run_frame_layout_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- FrameLayout ---\n";
    runner.run("frame_layout_values_are_distinct", frame_layout_values_are_distinct);
    runner.run("interleaved_is_not_planar", interleaved_is_not_planar);
    std::cout << "\n";
}
