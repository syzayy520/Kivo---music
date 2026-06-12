#include "../../fixture/output_truth_test_assert.hpp"
#include "../../fixture/output_truth_test_factory.hpp"
#include "../../runner/output_truth_test_cases.hpp"

#include "kivo/core/output/truth/evaluation/bit_perfect_truth_evaluator.hpp"

namespace output_truth_test {

void format_mismatches_are_classified() {
    using namespace kivo::core;
    auto sample_rate = clean_evidence();
    sample_rate.device_format = format(44100);
    OUTPUT_TRUTH_ASSERT(
        output::evaluate_bit_perfect_truth(sample_rate)
            .report.rejection_reason.category
        == contract::BitPerfectRejectionCategory::SampleRateMismatch);

    auto encoding = clean_evidence();
    auto changed = format();
    changed.sample_format = contract::SampleFormat::Int32;
    changed.bits_per_sample = 32;
    encoding.device_format = changed;
    OUTPUT_TRUTH_ASSERT(
        output::evaluate_bit_perfect_truth(encoding)
            .report.rejection_reason.category
        == contract::BitPerfectRejectionCategory::FormatMismatch);
}

void engine_resample_and_conversion_are_rejected() {
    using namespace kivo::core;
    auto engine = clean_evidence();
    engine.host_audio_engine_in_path = true;
    OUTPUT_TRUTH_ASSERT(
        output::evaluate_bit_perfect_truth(engine)
            .report.rejection_reason.category
        == contract::BitPerfectRejectionCategory::EngineParticipation);

    auto resample = clean_evidence();
    resample.resampler_active = true;
    OUTPUT_TRUTH_ASSERT(
        output::evaluate_bit_perfect_truth(resample)
            .report.rejection_reason.category
        == contract::BitPerfectRejectionCategory::ResamplingActive);

    using Activate = void (*)(output::BitPerfectEvidence&);
    const Activate conversions[] = {
        [](output::BitPerfectEvidence& value) {
            value.sample_format_conversion_active = true;
        },
        [](output::BitPerfectEvidence& value) {
            value.bit_depth_conversion_active = true;
        }
    };
    for (const auto activate : conversions) {
        auto conversion = clean_evidence();
        activate(conversion);
        const auto result = output::evaluate_bit_perfect_truth(conversion);
        OUTPUT_TRUTH_ASSERT(
            result.report.rejection_reason.category
            == contract::BitPerfectRejectionCategory::ConversionActive);
        OUTPUT_TRUTH_ASSERT(
            result.report.rejection_reason.conversion_active);
    }
}

} // namespace output_truth_test
