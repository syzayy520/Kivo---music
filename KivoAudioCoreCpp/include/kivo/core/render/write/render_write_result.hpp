#pragma once

#include <chrono>
#include <cstdint>

#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/render/failure/render_failure.hpp"

namespace kivo::core::render {

enum class RenderWriteDisposition : uint8_t {
    Full = 0,
    Partial,
    Delayed,
    Rejected,
    Failed
};

enum class UnwrittenFrameOwnership : uint8_t {
    None = 0,
    Caller
};

class [[nodiscard]] RenderWriteResult {
public:
    [[nodiscard]] static constexpr RenderWriteResult Full(
        contract::FrameCount submitted_frames) noexcept {
        if (submitted_frames == 0) {
            return Rejected(0, RenderFailure::InvalidBuffer);
        }
        return RenderWriteResult(
            RenderWriteDisposition::Full,
            RenderFailure::None,
            submitted_frames,
            submitted_frames,
            std::chrono::nanoseconds{0});
    }

    [[nodiscard]] static constexpr RenderWriteResult Partial(
        contract::FrameCount submitted_frames,
        contract::FrameCount accepted_frames) noexcept {
        if (submitted_frames == 0 || accepted_frames == 0 || accepted_frames >= submitted_frames) {
            return Rejected(submitted_frames, RenderFailure::InvalidBuffer);
        }
        return RenderWriteResult(
            RenderWriteDisposition::Partial,
            RenderFailure::None,
            submitted_frames,
            accepted_frames,
            std::chrono::nanoseconds{0});
    }

    [[nodiscard]] static constexpr RenderWriteResult Delayed(
        contract::FrameCount submitted_frames,
        std::chrono::nanoseconds retry_after) noexcept {
        if (submitted_frames == 0) {
            return Rejected(0, RenderFailure::InvalidBuffer);
        }
        return RenderWriteResult(
            RenderWriteDisposition::Delayed,
            RenderFailure::None,
            submitted_frames,
            0,
            retry_after.count() < 0 ? std::chrono::nanoseconds{0} : retry_after);
    }

    [[nodiscard]] static constexpr RenderWriteResult Rejected(
        contract::FrameCount submitted_frames,
        RenderFailure failure) noexcept {
        return RenderWriteResult(
            RenderWriteDisposition::Rejected,
            normalize_failure(failure),
            submitted_frames,
            0,
            std::chrono::nanoseconds{0});
    }

    [[nodiscard]] static constexpr RenderWriteResult Failed(
        contract::FrameCount submitted_frames,
        RenderFailure failure) noexcept {
        return RenderWriteResult(
            RenderWriteDisposition::Failed,
            normalize_failure(failure),
            submitted_frames,
            0,
            std::chrono::nanoseconds{0});
    }

    [[nodiscard]] constexpr RenderWriteDisposition disposition() const noexcept {
        return disposition_;
    }

    [[nodiscard]] constexpr RenderFailure failure() const noexcept {
        return failure_;
    }

    [[nodiscard]] constexpr contract::FrameCount submitted_frames() const noexcept {
        return submitted_frames_;
    }

    [[nodiscard]] constexpr contract::FrameCount accepted_frames() const noexcept {
        return accepted_frames_;
    }

    [[nodiscard]] constexpr contract::FrameCount unwritten_frames() const noexcept {
        return submitted_frames_ - accepted_frames_;
    }

    [[nodiscard]] constexpr UnwrittenFrameOwnership unwritten_ownership() const noexcept {
        return unwritten_frames() == 0
            ? UnwrittenFrameOwnership::None
            : UnwrittenFrameOwnership::Caller;
    }

    [[nodiscard]] constexpr std::chrono::nanoseconds retry_after() const noexcept {
        return retry_after_;
    }

    [[nodiscard]] constexpr bool operator==(const RenderWriteResult&) const noexcept = default;

private:
    constexpr RenderWriteResult(
        RenderWriteDisposition disposition,
        RenderFailure failure,
        contract::FrameCount submitted_frames,
        contract::FrameCount accepted_frames,
        std::chrono::nanoseconds retry_after) noexcept
        : disposition_(disposition),
          failure_(failure),
          submitted_frames_(submitted_frames),
          accepted_frames_(accepted_frames),
          retry_after_(retry_after) {}

    [[nodiscard]] static constexpr RenderFailure normalize_failure(RenderFailure failure) noexcept {
        return failure == RenderFailure::None ? RenderFailure::BoundaryFailure : failure;
    }

    RenderWriteDisposition disposition_;
    RenderFailure failure_;
    contract::FrameCount submitted_frames_;
    contract::FrameCount accepted_frames_;
    std::chrono::nanoseconds retry_after_;
};

} // namespace kivo::core::render
