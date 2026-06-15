#pragma once

#include "kivo/core/contract/generation/identity/device_generation.hpp"
#include "kivo/core/contract/generation/identity/flush_generation.hpp"
#include "kivo/core/contract/generation/identity/seek_generation.hpp"
#include "kivo/core/contract/generation/identity/stream_generation.hpp"

namespace kivo::core::render {

struct RenderGenerationSet {
    contract::StreamGeneration stream{};
    contract::SeekGenerationId seek{};
    contract::FlushGeneration flush{};
    contract::DeviceGeneration device{};

    [[nodiscard]] constexpr bool operator==(const RenderGenerationSet&) const noexcept = default;
};

} // namespace kivo::core::render
