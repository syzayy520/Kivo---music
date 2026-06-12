#include "../../fixture/hires_pcm_format_factory.hpp"
#include "../../runner/hires_pcm_test_cases.hpp"

#include <stdexcept>

#include "kivo/core/contract/format/dsd/dop_carrier_format.hpp"
#include "kivo/core/contract/format/dsd/native_dsd_format.hpp"
#include "kivo/core/decode/media/support/media_support_classification.hpp"

namespace {

#define ASSERT(cond) \
    do { \
        if (!(cond)) { \
            throw std::runtime_error("Assertion failed: " #cond); \
        } \
    } while (0)

} // namespace

namespace hires_pcm_test {

void pcm_sample_formats_remain_integer_or_float() {
    using namespace kivo::core::contract;

    const SampleFormat formats[] = {
        SampleFormat::Int16,
        SampleFormat::Int24,
        SampleFormat::Int32,
        SampleFormat::Float32,
        SampleFormat::Float64
    };

    for (const auto format : formats) {
        ASSERT(sample_format_valid_bits(format) != 0);
        ASSERT(sample_format_container_bits(format) != 0);
        ASSERT(sample_format_is_integer(format) || sample_format_is_float(format));
    }
}

void unsupported_non_pcm_payload_is_not_modelled_as_pcm() {
    using namespace kivo::core::contract;
    using namespace kivo::core::decode;

    const NativeDsdFormat native_dsd{DsdRate::Dsd64, 2};
    const DopCarrierFormat dop{DsdRate::Dsd64, 176400, 24, 32};
    const auto native_support =
        classify_media_support(AudioCodec::Dsd, MediaContainer::Dsf);
    const auto dop_support =
        classify_media_support(AudioCodec::Pcm, MediaContainer::Wave, true);

    ASSERT(native_dsd.is_valid());
    ASSERT(dop.is_valid());
    ASSERT(!native_support.is_supported());
    ASSERT(native_support.support
           == MediaPlaybackSupport::UnsupportedNativeDsd);
    ASSERT(!dop_support.is_supported());
    ASSERT(dop_support.support
           == MediaPlaybackSupport::UnsupportedDopCarrier);
}

} // namespace hires_pcm_test
