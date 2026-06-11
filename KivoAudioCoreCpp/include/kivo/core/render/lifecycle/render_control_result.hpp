#pragma once

#include <cstdint>

#include "kivo/core/render/failure/render_failure.hpp"
#include "kivo/core/render/lifecycle/render_lifecycle_state.hpp"

namespace kivo::core::render {

enum class RenderControlDisposition : uint8_t {
    Succeeded = 0,
    NoOp,
    Rejected,
    Failed,
    TimedOut
};

class [[nodiscard]] RenderControlResult {
public:
    [[nodiscard]] static constexpr RenderControlResult Succeeded(
        RenderLifecycleState state) noexcept {
        return RenderControlResult(RenderControlDisposition::Succeeded, RenderFailure::None, state);
    }

    [[nodiscard]] static constexpr RenderControlResult NoOp(
        RenderLifecycleState state) noexcept {
        return RenderControlResult(RenderControlDisposition::NoOp, RenderFailure::None, state);
    }

    [[nodiscard]] static constexpr RenderControlResult Rejected(
        RenderFailure failure,
        RenderLifecycleState state) noexcept {
        return RenderControlResult(
            RenderControlDisposition::Rejected,
            normalize_failure(failure),
            state);
    }

    [[nodiscard]] static constexpr RenderControlResult Failed(
        RenderFailure failure,
        RenderLifecycleState state = RenderLifecycleState::Failed) noexcept {
        return RenderControlResult(
            RenderControlDisposition::Failed,
            normalize_failure(failure),
            state);
    }

    [[nodiscard]] static constexpr RenderControlResult TimedOut(
        RenderLifecycleState state) noexcept {
        return RenderControlResult(
            RenderControlDisposition::TimedOut,
            RenderFailure::Timeout,
            state);
    }

    [[nodiscard]] constexpr RenderControlDisposition disposition() const noexcept {
        return disposition_;
    }

    [[nodiscard]] constexpr RenderFailure failure() const noexcept {
        return failure_;
    }

    [[nodiscard]] constexpr RenderLifecycleState state() const noexcept {
        return state_;
    }

    [[nodiscard]] constexpr bool is_success() const noexcept {
        return disposition_ == RenderControlDisposition::Succeeded
            || disposition_ == RenderControlDisposition::NoOp;
    }

    [[nodiscard]] constexpr bool operator==(const RenderControlResult&) const noexcept = default;

private:
    constexpr RenderControlResult(
        RenderControlDisposition disposition,
        RenderFailure failure,
        RenderLifecycleState state) noexcept
        : disposition_(disposition), failure_(failure), state_(state) {}

    [[nodiscard]] static constexpr RenderFailure normalize_failure(RenderFailure failure) noexcept {
        return failure == RenderFailure::None ? RenderFailure::BoundaryFailure : failure;
    }

    RenderControlDisposition disposition_;
    RenderFailure failure_;
    RenderLifecycleState state_;
};

} // namespace kivo::core::render
