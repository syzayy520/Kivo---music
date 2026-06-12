#pragma once

#include <cstdint>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/render/generation/render_generation_set.hpp"
#include "kivo/core/render/format/render_output_mode.hpp"

namespace kivo::core::render {

enum class FormatAcceptancePolicy : uint8_t {
    Exact = 0,
    AllowCompatible
};

struct RenderOpenRequest {
    contract::RenderFormat requested_format{};
    contract::FrameCount requested_buffer_frames{0};
    RenderOutputMode output_mode{RenderOutputMode::Shared};
    FormatAcceptancePolicy format_policy{FormatAcceptancePolicy::Exact};
    RenderGenerationSet generations{};

    [[nodiscard]] constexpr bool is_valid() const noexcept {
        return requested_format.is_valid() && requested_buffer_frames != 0;
    }

    [[nodiscard]] constexpr bool operator==(const RenderOpenRequest&) const noexcept = default;
};

} // namespace kivo::core::render
