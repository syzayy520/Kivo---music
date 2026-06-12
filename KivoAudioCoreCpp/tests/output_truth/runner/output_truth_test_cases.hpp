#pragma once

namespace output_truth_test {

void complete_clean_evidence_is_bit_perfect();
void incomplete_evidence_is_rejected();
void policy_forbidden_is_rejected();
void shared_and_mismatched_modes_are_rejected();
void format_mismatches_are_classified();
void engine_resample_and_conversion_are_rejected();
void processing_and_dither_are_rejected();

} // namespace output_truth_test
