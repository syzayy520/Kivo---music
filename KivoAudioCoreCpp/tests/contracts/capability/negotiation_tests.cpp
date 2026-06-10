// =============================================================================
// KivoAudioCoreCpp — negotiation_tests.cpp
// Tests for Capability negotiation contract types
// =============================================================================
//
// Tests: NegotiationCandidate, CandidatePresence, NegotiationResult,
//        NegotiationConstraints, NegotiationComparison
// =============================================================================

#include "kivo/core/contract/capability/negotiation/candidate.hpp"
#include "kivo/core/contract/capability/negotiation/candidate_presence.hpp"
#include "kivo/core/contract/capability/negotiation/result.hpp"
#include "kivo/core/contract/capability/negotiation/constraints.hpp"
#include "kivo/core/contract/capability/negotiation/comparison.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// CandidatePresence tests
// =============================================================================
static void presence_has_all_values() {
    ASSERT(CandidatePresence::Present != CandidatePresence::Absent);
    ASSERT(CandidatePresence::Absent != CandidatePresence::Unknown);
    ASSERT(CandidatePresence::Present != CandidatePresence::Unknown);
}

static void presence_equality() {
    ASSERT(CandidatePresence::Present == CandidatePresence::Present);
    ASSERT(CandidatePresence::Present != CandidatePresence::Absent);
}

// =============================================================================
// NegotiationCandidate tests
// =============================================================================
static void candidate_default_values() {
    NegotiationCandidate c;
    ASSERT(c.profile.scope == CapabilityScope::Decode);
    ASSERT(c.decode.decode_level == CapabilityLevel::None);
    ASSERT(c.render.render_level == CapabilityLevel::None);
    ASSERT(c.output.output_level == CapabilityLevel::None);
    ASSERT(c.meets_all_hard == false);
}

static void candidate_meets_all_hard_is_negotiation_outcome() {
    // meets_all_hard is a negotiation outcome, NOT a capability field
    NegotiationCandidate c;
    c.meets_all_hard = true;
    ASSERT(c.meets_all_hard);
    // It can change based on negotiation, not inherent capability
    c.meets_all_hard = false;
    ASSERT(!c.meets_all_hard);
}

static void candidate_equality() {
    NegotiationCandidate a;
    NegotiationCandidate b;
    ASSERT(a == b);
}

// =============================================================================
// NegotiationResult tests
// =============================================================================
static void result_default_values() {
    NegotiationResult r;
    ASSERT(r.presence == CandidatePresence::Unknown);
    ASSERT(r.best_candidate == nullptr);
    ASSERT(r.all_requirements_met == false);
}

static void result_with_candidate() {
    NegotiationCandidate c;
    c.decode.decode_level = CapabilityLevel::High;
    c.meets_all_hard = true;

    NegotiationResult r;
    r.presence = CandidatePresence::Present;
    r.best_candidate = &c;
    r.all_requirements_met = true;

    ASSERT(r.presence == CandidatePresence::Present);
    ASSERT(r.best_candidate != nullptr);
    ASSERT(r.best_candidate->decode.decode_level == CapabilityLevel::High);
    ASSERT(r.all_requirements_met);
}

static void result_absent_candidate() {
    NegotiationResult r;
    r.presence = CandidatePresence::Absent;
    r.best_candidate = nullptr;
    r.all_requirements_met = false;
    ASSERT(r.presence == CandidatePresence::Absent);
    ASSERT(r.best_candidate == nullptr);
}

// =============================================================================
// NegotiationConstraints tests
// =============================================================================
static void constraints_default_values() {
    NegotiationConstraints c;
    ASSERT(c.requirement.scope == CapabilityScope::Decode);
    ASSERT(c.requirement.minimum_level == CapabilityLevel::None);
    ASSERT(c.strictness == ConstraintStrictness::Required);
}

static void constraints_equality() {
    NegotiationConstraints a;
    NegotiationConstraints b;
    ASSERT(a == b);
}

// =============================================================================
// NegotiationComparison tests
// =============================================================================
static void comparison_outcome_values() {
    ASSERT(ComparisonOutcome::FirstBetter != ComparisonOutcome::SecondBetter);
    ASSERT(ComparisonOutcome::SecondBetter != ComparisonOutcome::Equivalent);
    ASSERT(ComparisonOutcome::Equivalent != ComparisonOutcome::Incomparable);
}

static void comparison_default_value() {
    NegotiationComparison c;
    ASSERT(c.outcome == ComparisonOutcome::Incomparable);
}

static void comparison_equality() {
    NegotiationComparison a{ComparisonOutcome::FirstBetter};
    NegotiationComparison b{ComparisonOutcome::FirstBetter};
    NegotiationComparison c{ComparisonOutcome::SecondBetter};
    ASSERT(a == b);
    ASSERT(a != c);
}

// =============================================================================
// Test runner
// =============================================================================
void run_negotiation_contract_tests(ContractTestRunner& runner) {
    runner.run("negotiation::presence_has_all_values", presence_has_all_values);
    runner.run("negotiation::presence_equality", presence_equality);
    runner.run("negotiation::candidate_default_values", candidate_default_values);
    runner.run("negotiation::candidate_meets_all_hard_is_negotiation_outcome", candidate_meets_all_hard_is_negotiation_outcome);
    runner.run("negotiation::candidate_equality", candidate_equality);
    runner.run("negotiation::result_default_values", result_default_values);
    runner.run("negotiation::result_with_candidate", result_with_candidate);
    runner.run("negotiation::result_absent_candidate", result_absent_candidate);
    runner.run("negotiation::constraints_default_values", constraints_default_values);
    runner.run("negotiation::constraints_equality", constraints_equality);
    runner.run("negotiation::comparison_outcome_values", comparison_outcome_values);
    runner.run("negotiation::comparison_default_value", comparison_default_value);
    runner.run("negotiation::comparison_equality", comparison_equality);
}
