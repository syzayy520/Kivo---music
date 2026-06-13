#pragma once

#include <cstdint>

#include "kivo/core/contract/capability/component/render.hpp"
#include "kivo/core/contract/capability/quality/level.hpp"
#include "kivo/core/contract/generation/identity/device_generation.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/render/identity/renderer_identity.hpp"

namespace kivo::core::render {

enum class RenderFeature : uint32_t {
    None = 0,
    PartialWrite = 1u << 0u,
    DelayReport = 1u << 1u,
    Flush = 1u << 2u,
    Drain = 1u << 3u,
    Reset = 1u << 4u,
    ExclusiveMode = 1u << 5u
};

[[nodiscard]] constexpr uint32_t render_feature_bit(RenderFeature feature) noexcept {
    return static_cast<uint32_t>(feature);
}

struct RendererCapabilitySnapshot {
    RendererIdentity identity{};
    contract::DeviceGeneration device_generation{};
    contract::RenderCapability render_capability{};
    contract::FrameCount buffer_capacity_frames{0};
    contract::FrameCount maximum_write_frames{0};
    uint32_t feature_bits{0};

    [[nodiscard]] constexpr bool supports(RenderFeature feature) const noexcept {
        const auto bit = render_feature_bit(feature);
        return bit != 0 && (feature_bits & bit) == bit;
    }

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return identity.is_valid()
            && render_capability.render_level != contract::CapabilityLevel::None
            && render_capability.max_sample_rate_ceiling != 0
            && render_capability.max_channels_ceiling != 0
            && buffer_capacity_frames != 0
            && maximum_write_frames != 0
            && maximum_write_frames <= buffer_capacity_frames;
    }

    [[nodiscard]] constexpr bool operator==(
        const RendererCapabilitySnapshot&) const noexcept = default;
};

} // namespace kivo::core::render
