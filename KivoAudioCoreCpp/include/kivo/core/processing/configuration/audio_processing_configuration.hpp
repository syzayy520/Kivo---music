#pragma once

#include <cstdint>

#include "kivo/core/processing/configuration/replay_gain_configuration.hpp"
#include "kivo/core/processing/configuration/software_volume_configuration.hpp"
#include "kivo/core/processing/policy/dither_policy.hpp"

namespace kivo::core::processing {

struct AudioProcessingConfiguration {
    ReplayGainConfiguration replay_gain{};
    SoftwareVolumeConfiguration volume{};
    DitherPolicy dither_policy{DitherPolicy::Disabled};
    uint64_t dither_seed{0x4B49564F41554449ull};
    bool bit_perfect_required{false};

    [[nodiscard]] bool is_valid() const noexcept {
        return replay_gain.is_valid()
            && volume.is_valid()
            && dither_seed != 0;
    }

    [[nodiscard]] bool mutation_requested() const noexcept {
        return replay_gain.enabled
            || volume.enabled
            || dither_policy != DitherPolicy::Disabled;
    }

    [[nodiscard]] bool operator==(
        const AudioProcessingConfiguration&) const noexcept = default;
};

} // namespace kivo::core::processing
