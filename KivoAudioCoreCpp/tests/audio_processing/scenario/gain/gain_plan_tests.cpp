#include "../../fixture/audio_processing_test_assert.hpp"
#include "../../fixture/audio_processing_test_factory.hpp"
#include "../../runner/audio_processing_test_cases.hpp"

#include <cmath>

#include "kivo/core/processing/result/audio_processing_plan.hpp"

namespace audio_processing_test {

void replay_gain_and_volume_are_independent() {
    using namespace kivo::core;
    processing::AudioProcessingConfiguration configuration{};
    configuration.replay_gain.enabled = true;
    configuration.replay_gain.gain_db = -6.020599913279624;
    configuration.volume.enabled = true;
    configuration.volume.linear_gain = 0.5;
    const auto plan = processing::build_audio_processing_plan(
        format(contract::SampleFormat::Float32),
        configuration);
    AUDIO_PROCESSING_ASSERT(plan.is_valid());
    AUDIO_PROCESSING_ASSERT(plan.participation.replay_gain_active);
    AUDIO_PROCESSING_ASSERT(plan.participation.volume_active);
    AUDIO_PROCESSING_ASSERT(std::abs(plan.replay_gain_linear - 0.5) < 1e-12);
    AUDIO_PROCESSING_ASSERT(std::abs(plan.effective_gain - 0.25) < 1e-12);
}

void peak_prevention_limits_gain() {
    using namespace kivo::core;
    processing::AudioProcessingConfiguration configuration{};
    configuration.replay_gain.enabled = true;
    configuration.replay_gain.gain_db = 12.0;
    configuration.replay_gain.peak = 0.8;
    const auto plan = processing::build_audio_processing_plan(
        format(contract::SampleFormat::Float32),
        configuration);
    AUDIO_PROCESSING_ASSERT(plan.is_valid());
    AUDIO_PROCESSING_ASSERT(
        std::abs(plan.replay_gain_linear - 1.25) < 1e-12);
}

} // namespace audio_processing_test
