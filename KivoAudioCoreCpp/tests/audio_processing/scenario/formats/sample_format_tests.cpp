#include "../../fixture/audio_processing_test_assert.hpp"
#include "../../fixture/audio_processing_test_factory.hpp"
#include "../../runner/audio_processing_test_cases.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <limits>

#include "kivo/core/processing/chain/audio_processing_chain.hpp"

namespace audio_processing_test {

template <size_t ByteSize>
void process(
    std::array<std::byte, ByteSize>& storage,
    kivo::core::contract::SampleFormat sample_format,
    double gain) {
    kivo::core::processing::AudioProcessingConfiguration configuration{};
    configuration.volume.enabled = true;
    configuration.volume.linear_gain = gain;
    auto chain = kivo::core::processing::AudioProcessingChain::create(
        format(sample_format),
        configuration);
    AUDIO_PROCESSING_ASSERT(chain);
    const auto frame_count =
        ByteSize / format(sample_format).format.bytes_per_frame();
    AUDIO_PROCESSING_ASSERT(
        chain->process(storage, frame_count).succeeded());
}

void integer_formats_apply_gain_and_saturate() {
    using kivo::core::contract::SampleFormat;
    auto int16 = bytes<int16_t>(std::array<int16_t, 2>{20000, -20000});
    process(int16, SampleFormat::Int16, 0.5);
    const auto int16_result = samples<int16_t>(int16);
    AUDIO_PROCESSING_ASSERT(int16_result[0] == 10000);
    AUDIO_PROCESSING_ASSERT(int16_result[1] == -10000);

    auto int24 = bytes<int32_t>(
        std::array<int32_t, 2>{4000000 * 256, -4000000 * 256});
    process(int24, SampleFormat::Int24, 0.5);
    const auto int24_result = samples<int32_t>(int24);
    AUDIO_PROCESSING_ASSERT(int24_result[0] == 2000000 * 256);
    AUDIO_PROCESSING_ASSERT(int24_result[1] == -2000000 * 256);

    auto int32 = bytes<int32_t>(
        std::array<int32_t, 2>{1000000000, -1000000000});
    process(int32, SampleFormat::Int32, 0.5);
    const auto int32_result = samples<int32_t>(int32);
    AUDIO_PROCESSING_ASSERT(int32_result[0] == 500000000);
    AUDIO_PROCESSING_ASSERT(int32_result[1] == -500000000);
}

void floating_formats_apply_gain_and_saturate() {
    using kivo::core::contract::SampleFormat;
    auto float32 = bytes<float>(std::array{0.8f, -0.8f});
    process(float32, SampleFormat::Float32, 0.5);
    const auto float32_result = samples<float>(float32);
    AUDIO_PROCESSING_ASSERT(std::abs(float32_result[0] - 0.4f) < 1e-6f);
    AUDIO_PROCESSING_ASSERT(std::abs(float32_result[1] + 0.4f) < 1e-6f);

    auto float64 = bytes<double>(std::array{0.75, -0.75});
    process(float64, SampleFormat::Float64, 0.5);
    const auto float64_result = samples<double>(float64);
    AUDIO_PROCESSING_ASSERT(std::abs(float64_result[0] - 0.375) < 1e-12);
    AUDIO_PROCESSING_ASSERT(std::abs(float64_result[1] + 0.375) < 1e-12);
}

} // namespace audio_processing_test
