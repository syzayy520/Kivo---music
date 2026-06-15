#include "../../fixture/hires_pcm_format_factory.hpp"
#include "../../runner/hires_pcm_test_cases.hpp"

#include <stdexcept>

namespace {

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error("Assertion failed: " #cond); \
        } \
    } while (0)

} // namespace

namespace hires_pcm_test {

void high_sample_rate_pcm_descriptors_are_valid() {
    using namespace kivo::core::contract;

    const uint32_t sample_rates[] = {88200, 96000, 176400, 192000, 384000};
    for (const auto sample_rate : sample_rates) {
        const auto int24 = make_pcm_format(SampleFormat::Int24,
                                           ChannelLayout::Stereo,
                                           sample_rate);
        const auto int32 = make_pcm_format(SampleFormat::Int32,
                                           ChannelLayout::Stereo,
                                           sample_rate);
        const auto float64 = make_pcm_format(SampleFormat::Float64,
                                             ChannelLayout::Stereo,
                                             sample_rate);

        ASSERT(int24.is_valid());
        ASSERT(int32.is_valid());
        ASSERT(float64.is_valid());
    }
}

void valid_bits_and_container_bits_remain_distinct() {
    using namespace kivo::core::contract;

    const auto packed = make_pcm_format(SampleFormat::Int24,
                                        ChannelLayout::Stereo,
                                        192000);

    ASSERT(packed.valid_bits_per_sample() == 24);
    ASSERT(packed.container_bits() == 32);
    ASSERT(packed.is_valid());
}

void frame_size_uses_container_bits() {
    using namespace kivo::core::contract;

    const auto stereo24 = make_pcm_format(SampleFormat::Int24,
                                          ChannelLayout::Stereo,
                                          384000);
    const auto surround32 = make_pcm_format(SampleFormat::Int32,
                                            ChannelLayout::Surround71,
                                            192000);

    ASSERT(stereo24.bytes_per_frame() == 8);
    ASSERT(surround32.bytes_per_frame() == 32);
}

} // namespace hires_pcm_test
