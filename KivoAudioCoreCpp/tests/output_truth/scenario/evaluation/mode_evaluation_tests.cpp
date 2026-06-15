#include "../../fixture/output_truth_test_assert.hpp"
#include "../../fixture/output_truth_test_factory.hpp"
#include "../../runner/output_truth_test_cases.hpp"

#include "kivo/core/output/truth/evaluation/bit_perfect_truth_evaluator.hpp"

namespace output_truth_test {

void complete_clean_evidence_is_bit_perfect() {
    const auto result = kivo::core::output::evaluate_bit_perfect_truth(
        clean_evidence());
    OUTPUT_TRUTH_ASSERT(result.is_bit_perfect());
    OUTPUT_TRUTH_ASSERT(result.report.evidence_complete);
    OUTPUT_TRUTH_ASSERT(result.report.requested_exclusive);
    OUTPUT_TRUTH_ASSERT(result.report.actual_exclusive);
    OUTPUT_TRUTH_ASSERT(result.report.resample.has_value());
    OUTPUT_TRUTH_ASSERT(!result.report.resample->is_needed());
}

void incomplete_evidence_is_rejected() {
    const auto result = kivo::core::output::evaluate_bit_perfect_truth({});
    OUTPUT_TRUTH_ASSERT(!result.is_bit_perfect());
    OUTPUT_TRUTH_ASSERT(!result.report.evidence_complete);
    OUTPUT_TRUTH_ASSERT(
        result.report.rejection_reason.category
        == kivo::core::contract::BitPerfectRejectionCategory::
            EvidenceIncomplete);
}

void policy_forbidden_is_rejected() {
    using namespace kivo::core;
    auto evidence = clean_evidence();
    evidence.policy_allows_bit_perfect = false;
    const auto result = output::evaluate_bit_perfect_truth(evidence);
    OUTPUT_TRUTH_ASSERT(!result.is_bit_perfect());
    OUTPUT_TRUTH_ASSERT(
        result.report.rejection_reason.category
        == contract::BitPerfectRejectionCategory::PolicyForbidden);
    OUTPUT_TRUTH_ASSERT(result.report.rejection_reason.policy_forbidden);
}

void shared_and_mismatched_modes_are_rejected() {
    using namespace kivo::core;
    auto shared = clean_evidence();
    shared.requested_mode = render::RenderOutputMode::Shared;
    shared.actual_mode = render::RenderOutputMode::Shared;
    OUTPUT_TRUTH_ASSERT(
        output::evaluate_bit_perfect_truth(shared)
            .report.rejection_reason.category
        == contract::BitPerfectRejectionCategory::SharedMode);

    auto mismatch = clean_evidence();
    mismatch.actual_mode = render::RenderOutputMode::Shared;
    OUTPUT_TRUTH_ASSERT(
        output::evaluate_bit_perfect_truth(mismatch)
            .report.rejection_reason.category
        == contract::BitPerfectRejectionCategory::ModeMismatch);
}

} // namespace output_truth_test
