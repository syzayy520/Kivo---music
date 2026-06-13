#pragma once

#include <cstdint>

#include "kivo/core/decode/failure/decode_failure.hpp"

namespace kivo::core::decode {

enum class DecodeControlDisposition : uint8_t {
    Succeeded = 0,
    NoOp,
    Unsupported,
    Rejected,
    Failed
};

struct DecodeControlResult {
    DecodeControlDisposition disposition{DecodeControlDisposition::NoOp};
    DecodeFailure failure{DecodeFailure::None};

    [[nodiscard]] static constexpr DecodeControlResult succeeded() noexcept {
        return {DecodeControlDisposition::Succeeded, DecodeFailure::None};
    }

    [[nodiscard]] static constexpr DecodeControlResult no_op() noexcept {
        return {DecodeControlDisposition::NoOp, DecodeFailure::None};
    }

    [[nodiscard]] static constexpr DecodeControlResult unsupported(
        DecodeFailure failure) noexcept {
        return {
            DecodeControlDisposition::Unsupported,
            normalize_failure(failure)
        };
    }

    [[nodiscard]] static constexpr DecodeControlResult rejected(
        DecodeFailure failure) noexcept {
        return {DecodeControlDisposition::Rejected, normalize_failure(failure)};
    }

    [[nodiscard]] static constexpr DecodeControlResult failed(
        DecodeFailure failure) noexcept {
        return {DecodeControlDisposition::Failed, normalize_failure(failure)};
    }

    [[nodiscard]] constexpr bool is_success() const noexcept {
        return disposition == DecodeControlDisposition::Succeeded
            || disposition == DecodeControlDisposition::NoOp;
    }

    [[nodiscard]] constexpr bool operator==(
        const DecodeControlResult&) const noexcept = default;

private:
    [[nodiscard]] static constexpr DecodeFailure normalize_failure(
        DecodeFailure failure) noexcept {
        return failure == DecodeFailure::None
            ? DecodeFailure::BoundaryFailure
            : failure;
    }
};

} // namespace kivo::core::decode
