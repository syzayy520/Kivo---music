#pragma once

#include <chrono>
#include <cstddef>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/render/capability/renderer_capability_snapshot.hpp"
#include "kivo/testing/render/observation/render_observation.hpp"

namespace kivo::testing::render {

struct FakeRendererConfiguration {
    core::contract::RenderFormat supported_format{};
    core::render::RendererCapabilitySnapshot capabilities{};
    size_t observation_capacity{64};
    std::chrono::nanoseconds delayed_write_retry{std::chrono::milliseconds{1}};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return supported_format.is_valid()
            && capabilities.is_valid()
            && observation_capacity != 0
            && observation_capacity <= kMaximumObservationHistoryCapacity
            && delayed_write_retry.count() >= 0;
    }

    [[nodiscard]] constexpr bool operator==(
        const FakeRendererConfiguration&) const noexcept = default;
};

} // namespace kivo::testing::render
