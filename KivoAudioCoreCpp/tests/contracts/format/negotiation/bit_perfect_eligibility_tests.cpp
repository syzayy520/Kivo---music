// =============================================================================
// KivoAudioCoreCpp — bit_perfect_eligibility_tests.cpp
// Tests for BitPerfectEligibility contract type
// =============================================================================

#include "kivo/core/contract/format/negotiation/bit_perfect_eligibility.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_is_unknown() {
    BitPerfectEligibility eligibility;
    ASSERT(eligibility.status == BitPerfectStatus::Unknown);
    ASSERT(!eligibility.is_eligible());
}

static void eligible_status() {
    BitPerfectEligibility eligibility;
    eligibility.status = BitPerfectStatus::Eligible;
    eligibility.formats_match = true;
    eligibility.no_conversion_needed = true;
    eligibility.no_mixing_needed = true;
    ASSERT(eligibility.is_eligible());
}

static void ineligible_status() {
    BitPerfectEligibility eligibility;
    eligibility.status = BitPerfectStatus::Ineligible;
    eligibility.formats_match = false;
    ASSERT(!eligibility.is_eligible());
}

static void blocked_by_policy() {
    BitPerfectEligibility eligibility;
    eligibility.status = BitPerfectStatus::BlockedByPolicy;
    ASSERT(!eligibility.is_eligible());
}

static void equality_same() {
    BitPerfectEligibility a;
    a.status = BitPerfectStatus::Eligible;
    a.formats_match = true;

    BitPerfectEligibility b = a;
    ASSERT(a == b);
}

static void equality_different() {
    BitPerfectEligibility a;
    a.status = BitPerfectStatus::Eligible;

    BitPerfectEligibility b;
    b.status = BitPerfectStatus::Ineligible;

    ASSERT(!(a == b));
}

void run_bit_perfect_eligibility_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- BitPerfectEligibility ---\n";
    runner.run("default_is_unknown", default_is_unknown);
    runner.run("eligible_status", eligible_status);
    runner.run("ineligible_status", ineligible_status);
    runner.run("blocked_by_policy", blocked_by_policy);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
