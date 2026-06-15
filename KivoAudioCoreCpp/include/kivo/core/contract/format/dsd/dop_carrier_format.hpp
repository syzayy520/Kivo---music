#pragma once

#include <cstdint>

#include "dop_marker.hpp"
#include "dsd_rate.hpp"

namespace kivo::core::contract {

struct DopCarrierFormat {
    DsdRate payload_rate{DsdRate::Unknown};
    uint32_t carrier_sample_rate{0};
    uint8_t carrier_valid_bits_per_sample{24};
    uint8_t carrier_container_bits_per_sample{24};
    bool markers_alternate_per_sample{true};
    bool markers_synchronized_across_channels{true};
    DopMarkerPair marker_pair{};

    [[nodiscard]] constexpr uint32_t expected_carrier_sample_rate() const noexcept {
        return dsd_bit_rate(payload_rate) / 16u;
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return dsd_rate_is_known(payload_rate)
            && carrier_sample_rate == expected_carrier_sample_rate()
            && carrier_valid_bits_per_sample == 24
            && (carrier_container_bits_per_sample == 24
                || carrier_container_bits_per_sample == 32)
            && carrier_valid_bits_per_sample
                <= carrier_container_bits_per_sample
            && markers_alternate_per_sample
            && markers_synchronized_across_channels
            && marker_pair.is_valid();
    }

    [[nodiscard]] constexpr bool operator==(const DopCarrierFormat&) const noexcept = default;
};

} // namespace kivo::core::contract
