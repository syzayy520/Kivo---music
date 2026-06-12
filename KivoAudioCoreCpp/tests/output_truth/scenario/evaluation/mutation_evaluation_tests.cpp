#include "../../fixture/output_truth_test_assert.hpp"
#include "../../fixture/output_truth_test_factory.hpp"
#include "../../runner/output_truth_test_cases.hpp"

#include <array>

#include "kivo/core/output/truth/evaluation/bit_perfect_truth_evaluator.hpp"

namespace output_truth_test {

void processing_and_dither_are_rejected() {
    using namespace kivo::core;
    using Category = contract::BitPerfectRejectionCategory;
    struct Case {
        Category expected;
        void (*activate)(output::BitPerfectEvidence&);
    };
    const std::array<Case, 5> cases{{
        {Category::ProcessingActive, [](auto& value) {
             value.channel_remix_active = true;
         }},
        {Category::ReplayGainActive, [](auto& value) {
             value.replay_gain_active = true;
         }},
        {Category::VolumeActive, [](auto& value) {
             value.volume_active = true;
         }},
        {Category::DspActive, [](auto& value) {
             value.dsp_active = true;
         }},
        {Category::DitherActive, [](auto& value) {
             value.dither_active = true;
         }}
    }};
    for (const auto& test : cases) {
        auto evidence = clean_evidence();
        test.activate(evidence);
        const auto result = output::evaluate_bit_perfect_truth(evidence);
        OUTPUT_TRUTH_ASSERT(!result.is_bit_perfect());
        OUTPUT_TRUTH_ASSERT(
            result.report.rejection_reason.category == test.expected);
    }
}

} // namespace output_truth_test
