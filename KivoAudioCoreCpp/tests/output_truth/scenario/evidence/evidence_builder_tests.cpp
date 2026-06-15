#include "../../fixture/output_truth_test_assert.hpp"
#include "../../fixture/output_truth_test_factory.hpp"
#include "../../runner/output_truth_test_cases.hpp"

#include "kivo/core/output/truth/evaluation/bit_perfect_truth_evaluator.hpp"
#include "kivo/core/output/truth/evidence/bit_perfect_evidence_builder.hpp"

namespace output_truth_test {

void snapshots_build_complete_truth_evidence() {
    using namespace kivo::core;

    processing::AudioConversionSnapshot conversion{};
    processing::AudioProcessingSnapshot processing{};
    const auto descriptor = format();
    const auto clean = output::build_bit_perfect_evidence(
        render::RenderOutputMode::Exclusive,
        render::RenderOutputMode::Exclusive,
        descriptor,
        descriptor,
        descriptor,
        false,
        true,
        conversion,
        processing);
    OUTPUT_TRUTH_ASSERT(clean.is_complete());
    OUTPUT_TRUTH_ASSERT(
        output::evaluate_bit_perfect_truth(clean).is_bit_perfect());

    conversion.conversion.sample_rate_convert = true;
    processing.participation.volume_active = true;
    processing.dither_active = true;
    const auto mutated = output::build_bit_perfect_evidence(
        render::RenderOutputMode::Exclusive,
        render::RenderOutputMode::Exclusive,
        descriptor,
        descriptor,
        descriptor,
        false,
        true,
        conversion,
        processing);
    OUTPUT_TRUTH_ASSERT(*mutated.resampler_active);
    OUTPUT_TRUTH_ASSERT(*mutated.volume_active);
    OUTPUT_TRUTH_ASSERT(*mutated.dither_active);
}

} // namespace output_truth_test
