#include "../../fixture/audio_processing_test_assert.hpp"
#include "../../fixture/audio_processing_test_factory.hpp"
#include "../../runner/audio_processing_test_cases.hpp"

#include <array>

#include "kivo/core/processing/chain/audio_processing_chain.hpp"

namespace audio_processing_test {

void default_bypass_is_byte_identical() {
    using kivo::core::contract::SampleFormat;
    const SampleFormat formats[] = {
        SampleFormat::Int16,
        SampleFormat::Int24,
        SampleFormat::Int32,
        SampleFormat::Float32,
        SampleFormat::Float64
    };
    for (const auto sample_format : formats) {
        std::array<std::byte, 32> storage{};
        for (size_t index = 0; index < storage.size(); ++index) {
            storage[index] = static_cast<std::byte>(index + 1);
        }
        const auto original = storage;
        auto chain = kivo::core::processing::AudioProcessingChain::create(
            format(sample_format),
            {});
        AUDIO_PROCESSING_ASSERT(chain);
        const auto bytes_per_frame =
            format(sample_format).format.bytes_per_frame();
        const auto frame_count = storage.size() / bytes_per_frame;
        const auto result = chain->process(
            std::span<std::byte>{storage}.first(frame_count * bytes_per_frame),
            frame_count);
        AUDIO_PROCESSING_ASSERT(
            result.disposition
            == kivo::core::processing::AudioProcessingDisposition::Bypassed);
        AUDIO_PROCESSING_ASSERT(storage == original);
        AUDIO_PROCESSING_ASSERT(chain->snapshot().processed_blocks == 0);
    }
}

void unity_enabled_stages_are_bypassed() {
    using namespace kivo::core;
    processing::AudioProcessingConfiguration configuration{};
    configuration.replay_gain.enabled = true;
    configuration.volume.enabled = true;
    auto chain = processing::AudioProcessingChain::create(
        format(contract::SampleFormat::Float32),
        configuration);
    AUDIO_PROCESSING_ASSERT(chain);
    auto storage = bytes<float>(std::array{0.25f, -0.25f});
    const auto original = storage;
    const auto result = chain->process(storage, 1);
    AUDIO_PROCESSING_ASSERT(
        result.disposition
        == processing::AudioProcessingDisposition::Bypassed);
    AUDIO_PROCESSING_ASSERT(storage == original);
}

} // namespace audio_processing_test
