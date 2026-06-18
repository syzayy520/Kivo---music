#include "../../fixture/audio_processing_test_assert.hpp"
#include "../../fixture/audio_processing_test_factory.hpp"
#include "../../runner/audio_processing_test_cases.hpp"

#include <cmath>

#include "kivo/core/processing/chain/audio_processing_chain.hpp"
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

void chain_set_volume_recomputes_gain_dither_and_bypass() {
    using namespace kivo::core;
    processing::AudioProcessingConfiguration configuration{};
    configuration.volume.enabled = true;
    configuration.volume.linear_gain = 1.0;  // unity → bypassed at open
    configuration.dither_policy = processing::DitherPolicy::TriangularPdf;
    auto chain = processing::AudioProcessingChain::create(
        format(contract::SampleFormat::Int16),
        configuration);
    AUDIO_PROCESSING_ASSERT(chain != nullptr);
    {
        const auto snapshot = chain->snapshot();
        AUDIO_PROCESSING_ASSERT(!snapshot.participation.volume_active);
        AUDIO_PROCESSING_ASSERT(snapshot.strict_bypass);
        AUDIO_PROCESSING_ASSERT(!snapshot.dither_active);
    }
    // Non-unity → volume active, bypass lifts, dither engages (Int16 + TPDF).
    AUDIO_PROCESSING_ASSERT(chain->set_volume(0.5));
    {
        const auto snapshot = chain->snapshot();
        AUDIO_PROCESSING_ASSERT(snapshot.participation.volume_active);
        AUDIO_PROCESSING_ASSERT(!snapshot.strict_bypass);
        AUDIO_PROCESSING_ASSERT(snapshot.dither_active);
        AUDIO_PROCESSING_ASSERT(
            std::abs(snapshot.effective_gain - 0.5) < 1e-12);
    }
    // Back to unity → all of volume_active / dither_active / bypass restored.
    AUDIO_PROCESSING_ASSERT(chain->set_volume(1.0));
    {
        const auto snapshot = chain->snapshot();
        AUDIO_PROCESSING_ASSERT(!snapshot.participation.volume_active);
        AUDIO_PROCESSING_ASSERT(snapshot.strict_bypass);
        AUDIO_PROCESSING_ASSERT(!snapshot.dither_active);
    }
    // Non-finite is rejected and leaves the chain unchanged.
    AUDIO_PROCESSING_ASSERT(chain->set_volume(0.25));
    AUDIO_PROCESSING_ASSERT(!chain->set_volume(std::nan("")));
    AUDIO_PROCESSING_ASSERT(
        std::abs(chain->snapshot().effective_gain - 0.25) < 1e-12);
}

void chain_set_volume_refuses_under_bit_perfect() {
    using namespace kivo::core;
    processing::AudioProcessingConfiguration configuration{};
    configuration.bit_perfect_required = true;  // no mutations → valid bypass
    auto chain = processing::AudioProcessingChain::create(
        format(contract::SampleFormat::Float32),
        configuration);
    AUDIO_PROCESSING_ASSERT(chain != nullptr);
    AUDIO_PROCESSING_ASSERT(chain->snapshot().strict_bypass);
    // A runtime volume change would break bit-perfect → refused, chain intact.
    AUDIO_PROCESSING_ASSERT(!chain->set_volume(0.5));
    AUDIO_PROCESSING_ASSERT(chain->snapshot().strict_bypass);
    AUDIO_PROCESSING_ASSERT(!chain->snapshot().participation.volume_active);
}

} // namespace audio_processing_test
