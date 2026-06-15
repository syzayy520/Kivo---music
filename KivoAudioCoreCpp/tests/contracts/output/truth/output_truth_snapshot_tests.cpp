// =============================================================================
// KivoAudioCoreCpp — output_truth_snapshot_tests.cpp
// Tests for OutputTruthSnapshot contract type
// =============================================================================

#include "kivo/core/contract/output/truth/output_truth_snapshot.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_snapshot_is_not_bit_perfect() {
    OutputTruthSnapshot snapshot;
    ASSERT(!snapshot.is_bit_perfect());
}

static void bit_perfect_via_report() {
    OutputTruthSnapshot snapshot;
    snapshot.bitperfect.bit_perfect_achieved = true;
    snapshot.bitperfect.evidence_complete = true;
    snapshot.bitperfect.requested_exclusive = true;
    snapshot.bitperfect.actual_exclusive = true;
    snapshot.bitperfect.formats_match = true;
    snapshot.bitperfect.no_processing_active = true;
    snapshot.bitperfect.no_engine_in_path = true;
    snapshot.bitperfect.no_sample_mutation = true;
    ASSERT(snapshot.is_bit_perfect());
}

static void equality_same() {
    OutputTruthSnapshot a;
    a.bitperfect.bit_perfect_achieved = true;
    OutputTruthSnapshot b = a;
    ASSERT(a == b);
}

static void equality_different() {
    OutputTruthSnapshot a;
    a.processing.replay_gain_active = true;
    OutputTruthSnapshot b;
    b.processing.replay_gain_active = false;
    ASSERT(!(a == b));
}

void run_output_truth_snapshot_tests(ContractTestRunner& runner) {
    std::cout << "--- OutputTruthSnapshot ---\n";
    runner.run("default_snapshot_is_not_bit_perfect", default_snapshot_is_not_bit_perfect);
    runner.run("bit_perfect_via_report", bit_perfect_via_report);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
