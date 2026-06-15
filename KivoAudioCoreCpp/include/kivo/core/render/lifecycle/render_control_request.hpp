#pragma once

#include <chrono>

#include "kivo/core/render/generation/render_generation_set.hpp"

namespace kivo::core::render {

struct RenderFlushRequest {
    RenderGenerationSet generations{};

    [[nodiscard]] constexpr bool operator==(const RenderFlushRequest&) const noexcept = default;
};

struct RenderDrainRequest {
    std::chrono::nanoseconds timeout{0};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return timeout.count() >= 0;
    }

    [[nodiscard]] constexpr bool operator==(const RenderDrainRequest&) const noexcept = default;
};

struct RenderResetRequest {
    RenderGenerationSet generations{};

    [[nodiscard]] constexpr bool operator==(const RenderResetRequest&) const noexcept = default;
};

} // namespace kivo::core::render
