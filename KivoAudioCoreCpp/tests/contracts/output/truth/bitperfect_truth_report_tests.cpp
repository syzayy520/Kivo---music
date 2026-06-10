// =============================================================================
// KivoAudioCoreCpp — bitperfect_truth_report_tests.cpp
// Tests for BitPerfectTruthReport contract type
// =============================================================================

#include "kivo/core/contract/output/truth/bitperfect_truth_report.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_report_not_bit_perfect() {
    BitPerfectTruthReport report;
    ASSERT(!report.is_bit_perfect());
    ASSERT(!report.bit_perfect_achieved);
    ASSERT(!report.was_rejected());
}

static void achieved_bit_perfect() {
    BitPerfectTruthReport report;
    report.bit_perfect_achieved = true;
    report.formats_match = true;
    report.no_processing_active = true;
    report.no_engine_in_path = true;
    ASSERT(report.is_bit_perfect());
    ASSERT(!report.was_rejected());
}

static void rejected_with_reason() {
    BitPerfectTruthReport report;
    report.bit_perfect_achieved = false;
    report.rejection_reason.category = BitPerfectRejectionCategory::FormatMismatch;
    report.rejection_reason.format_mismatch = true;
    ASSERT(!report.is_bit_perfect());
    ASSERT(report.was_rejected());
}

static void resample_optional_empty() {
    BitPerfectTruthReport report;
    ASSERT(!report.resample.has_value());
}

static void resample_optional_present() {
    BitPerfectTruthReport report;
    ResampleDecision rd;
    rd.resample = true;
    rd.reason = ResampleReason::DeviceMismatch;
    rd.source_rate = 44100;
    rd.target_rate = 48000;
    report.resample = rd;
    ASSERT(report.resample.has_value());
    ASSERT(report.resample->is_needed());
}

static void equality_same() {
    BitPerfectTruthReport a;
    a.bit_perfect_achieved = true;
    BitPerfectTruthReport b = a;
    ASSERT(a == b);
}

static void equality_different() {
    BitPerfectTruthReport a;
    a.bit_perfect_achieved = true;
    BitPerfectTruthReport b;
    b.bit_perfect_achieved = false;
    ASSERT(!(a == b));
}

void run_bitperfect_truth_report_tests(ContractTestRunner& runner) {
    std::cout << "--- BitPerfectTruthReport ---\n";
    runner.run("default_report_not_bit_perfect", default_report_not_bit_perfect);
    runner.run("achieved_bit_perfect", achieved_bit_perfect);
    runner.run("rejected_with_reason", rejected_with_reason);
    runner.run("resample_optional_empty", resample_optional_empty);
    runner.run("resample_optional_present", resample_optional_present);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
