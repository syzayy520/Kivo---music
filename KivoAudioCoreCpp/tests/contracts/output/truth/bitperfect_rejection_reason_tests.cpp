// =============================================================================
// KivoAudioCoreCpp — bitperfect_rejection_reason_tests.cpp
// Tests for BitPerfectRejectionReason contract type
// =============================================================================

#include "kivo/core/contract/output/truth/bitperfect_rejection_reason.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_has_no_reason() {
    BitPerfectRejectionReason reason;
    ASSERT(!reason.has_reason());
    ASSERT(reason.category == BitPerfectRejectionCategory::Unknown);
}

static void format_mismatch_category() {
    BitPerfectRejectionReason reason;
    reason.category = BitPerfectRejectionCategory::FormatMismatch;
    reason.format_mismatch = true;
    ASSERT(reason.has_reason());
    ASSERT(reason.format_mismatch);
}

static void processing_active_category() {
    BitPerfectRejectionReason reason;
    reason.category = BitPerfectRejectionCategory::ProcessingActive;
    reason.processing_active = true;
    ASSERT(reason.has_reason());
    ASSERT(reason.processing_active);
}

static void policy_forbidden_category() {
    BitPerfectRejectionReason reason;
    reason.category = BitPerfectRejectionCategory::PolicyForbidden;
    reason.policy_forbidden = true;
    ASSERT(reason.has_reason());
}

static void equality_same() {
    BitPerfectRejectionReason a;
    a.category = BitPerfectRejectionCategory::FormatMismatch;
    a.format_mismatch = true;
    BitPerfectRejectionReason b = a;
    ASSERT(a == b);
}

static void equality_different() {
    BitPerfectRejectionReason a;
    a.category = BitPerfectRejectionCategory::FormatMismatch;
    BitPerfectRejectionReason b;
    b.category = BitPerfectRejectionCategory::ProcessingActive;
    ASSERT(!(a == b));
}

void run_bitperfect_rejection_reason_tests(ContractTestRunner& runner) {
    std::cout << "--- BitPerfectRejectionReason ---\n";
    runner.run("default_has_no_reason", default_has_no_reason);
    runner.run("format_mismatch_category", format_mismatch_category);
    runner.run("processing_active_category", processing_active_category);
    runner.run("policy_forbidden_category", policy_forbidden_category);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
