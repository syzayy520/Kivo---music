#include "../../fixture/audio_processing_test_assert.hpp"
#include "../../fixture/audio_processing_test_factory.hpp"
#include "../../runner/audio_processing_test_cases.hpp"

#include <array>
#include <limits>

#include "kivo/core/processing/chain/audio_processing_chain.hpp"
#include "kivo/core/processing/result/audio_processing_plan.hpp"

namespace audio_processing_test {

void triangular_dither_is_deterministic() {
    using namespace kivo::core;
    processing::AudioProcessingConfiguration configuration{};
    configuration.volume.enabled = true;
    configuration.volume.linear_gain = 0.5;
    configuration.dither_policy = processing::DitherPolicy::TriangularPdf;
    configuration.dither_seed = 42;
    auto first = processing::AudioProcessingChain::create(
        format(contract::SampleFormat::Int16),
        configuration);
    auto second = processing::AudioProcessingChain::create(
        format(contract::SampleFormat::Int16),
        configuration);
    AUDIO_PROCESSING_ASSERT(first && second);
    auto first_bytes = bytes<int16_t>(std::array<int16_t, 4>{1, 1, 1, 1});
    auto second_bytes = first_bytes;
    AUDIO_PROCESSING_ASSERT(first->process(first_bytes, 2).succeeded());
    AUDIO_PROCESSING_ASSERT(second->process(second_bytes, 2).succeeded());
    AUDIO_PROCESSING_ASSERT(first_bytes == second_bytes);
    AUDIO_PROCESSING_ASSERT(first->snapshot().dither_active);
}

void bit_perfect_conflicts_are_rejected() {
    using namespace kivo::core;
    processing::AudioProcessingConfiguration configuration{};
    configuration.bit_perfect_required = true;
    configuration.volume.enabled = true;
    configuration.volume.linear_gain = 0.5;
    const auto plan = processing::build_audio_processing_plan(
        format(contract::SampleFormat::Float32),
        configuration);
    AUDIO_PROCESSING_ASSERT(
        plan.failure == processing::AudioProcessingFailure::BitPerfectConflict);
    AUDIO_PROCESSING_ASSERT(!processing::AudioProcessingChain::create(
        format(contract::SampleFormat::Float32),
        configuration));
}

void invalid_policy_is_rejected() {
    using namespace kivo::core;
    processing::AudioProcessingConfiguration configuration{};
    configuration.volume.linear_gain =
        std::numeric_limits<double>::quiet_NaN();
    const auto plan = processing::build_audio_processing_plan(
        format(contract::SampleFormat::Float32),
        configuration);
    AUDIO_PROCESSING_ASSERT(
        plan.failure
        == processing::AudioProcessingFailure::InvalidConfiguration);
}

} // namespace audio_processing_test
