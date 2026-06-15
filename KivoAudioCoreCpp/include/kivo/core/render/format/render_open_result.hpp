#pragma once

#include <cstdint>

#include "kivo/core/contract/format/roles/render_format.hpp"
#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/render/capability/renderer_capability_snapshot.hpp"
#include "kivo/core/render/failure/render_failure.hpp"

namespace kivo::core::render {

enum class RenderOpenDisposition : uint8_t {
    Accepted = 0,
    Rejected,
    Failed
};

class [[nodiscard]] RenderOpenResult {
public:
    [[nodiscard]] static constexpr RenderOpenResult Accepted(
        contract::RenderFormat format,
        contract::FrameCount buffer_frames,
        RendererCapabilitySnapshot capabilities) noexcept {
        if (!format.is_valid()
            || buffer_frames == 0
            || !capabilities.is_valid()
            || buffer_frames > capabilities.buffer_capacity_frames) {
            return Failed(RenderFailure::BoundaryFailure);
        }
        return RenderOpenResult(
            RenderOpenDisposition::Accepted,
            RenderFailure::None,
            format,
            buffer_frames,
            capabilities);
    }

    [[nodiscard]] static constexpr RenderOpenResult Rejected(RenderFailure failure) noexcept {
        return RenderOpenResult(
            RenderOpenDisposition::Rejected,
            normalize_failure(failure),
            {},
            0,
            {});
    }

    [[nodiscard]] static constexpr RenderOpenResult Failed(RenderFailure failure) noexcept {
        return RenderOpenResult(
            RenderOpenDisposition::Failed,
            normalize_failure(failure),
            {},
            0,
            {});
    }

    [[nodiscard]] constexpr RenderOpenDisposition disposition() const noexcept {
        return disposition_;
    }

    [[nodiscard]] constexpr RenderFailure failure() const noexcept {
        return failure_;
    }

    [[nodiscard]] constexpr const contract::RenderFormat& accepted_format() const noexcept {
        return accepted_format_;
    }

    [[nodiscard]] constexpr contract::FrameCount buffer_frames() const noexcept {
        return buffer_frames_;
    }

    [[nodiscard]] constexpr const RendererCapabilitySnapshot& capabilities() const noexcept {
        return capabilities_;
    }

    [[nodiscard]] constexpr bool is_accepted() const noexcept {
        return disposition_ == RenderOpenDisposition::Accepted;
    }

    [[nodiscard]] constexpr bool operator==(const RenderOpenResult&) const noexcept = default;

private:
    constexpr RenderOpenResult(
        RenderOpenDisposition disposition,
        RenderFailure failure,
        contract::RenderFormat format,
        contract::FrameCount buffer_frames,
        RendererCapabilitySnapshot capabilities) noexcept
        : disposition_(disposition),
          failure_(failure),
          accepted_format_(format),
          buffer_frames_(buffer_frames),
          capabilities_(capabilities) {}

    [[nodiscard]] static constexpr RenderFailure normalize_failure(RenderFailure failure) noexcept {
        return failure == RenderFailure::None ? RenderFailure::BoundaryFailure : failure;
    }

    RenderOpenDisposition disposition_;
    RenderFailure failure_;
    contract::RenderFormat accepted_format_;
    contract::FrameCount buffer_frames_;
    RendererCapabilitySnapshot capabilities_;
};

} // namespace kivo::core::render
