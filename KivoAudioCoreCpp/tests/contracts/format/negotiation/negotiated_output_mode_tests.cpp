// =============================================================================
// KivoAudioCoreCpp — negotiated_output_mode_tests.cpp
// Tests for NegotiatedOutputMode contract type
// =============================================================================

#include "kivo/core/contract/format/negotiation/negotiated_output_mode.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_mode_is_invalid() {
    NegotiatedOutputMode mode;
    ASSERT(!mode.is_valid());
    ASSERT(mode.mode == OutputMode::Unknown);
}

static void shared_mode_is_valid() {
    NegotiatedOutputMode mode;
    mode.mode = OutputMode::Shared;
    ASSERT(mode.is_valid());
    ASSERT(!mode.is_bit_perfect());
}

static void bit_perfect_active_is_bit_perfect() {
    NegotiatedOutputMode mode;
    mode.mode = OutputMode::BitPerfectActive;
    mode.bit_perfect_active = true;
    ASSERT(mode.is_valid());
    ASSERT(mode.is_bit_perfect());
}

static void bit_perfect_rejected_is_not_bit_perfect() {
    NegotiatedOutputMode mode;
    mode.mode = OutputMode::BitPerfectRejected;
    mode.bit_perfect_active = false;
    ASSERT(mode.is_valid());
    ASSERT(!mode.is_bit_perfect());
}

static void bit_perfect_with_resample_not_bit_perfect() {
    NegotiatedOutputMode mode;
    mode.mode = OutputMode::BitPerfectActive;
    mode.bit_perfect_active = true;
    mode.resampling_active = true;
    ASSERT(!mode.is_bit_perfect());
}

static void bit_perfect_with_conversion_not_bit_perfect() {
    NegotiatedOutputMode mode;
    mode.mode = OutputMode::BitPerfectActive;
    mode.bit_perfect_active = true;
    mode.conversion_active = true;
    ASSERT(!mode.is_bit_perfect());
}

static void equality_same() {
    NegotiatedOutputMode a;
    a.mode = OutputMode::Shared;

    NegotiatedOutputMode b = a;
    ASSERT(a == b);
}

static void equality_different() {
    NegotiatedOutputMode a;
    a.mode = OutputMode::Shared;

    NegotiatedOutputMode b;
    b.mode = OutputMode::Exclusive;

    ASSERT(!(a == b));
}

void run_negotiated_output_mode_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- NegotiatedOutputMode ---\n";
    runner.run("default_mode_is_invalid", default_mode_is_invalid);
    runner.run("shared_mode_is_valid", shared_mode_is_valid);
    runner.run("bit_perfect_active_is_bit_perfect", bit_perfect_active_is_bit_perfect);
    runner.run("bit_perfect_rejected_is_not_bit_perfect", bit_perfect_rejected_is_not_bit_perfect);
    runner.run("bit_perfect_with_resample_not_bit_perfect", bit_perfect_with_resample_not_bit_perfect);
    runner.run("bit_perfect_with_conversion_not_bit_perfect", bit_perfect_with_conversion_not_bit_perfect);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
