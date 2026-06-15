#pragma once

#include <cstdint>

#include "kivo/core/decode/failure/decode_failure.hpp"
#include "kivo/core/decode/media/media_probe.hpp"

namespace kivo::core::decode {

enum class DecodeOpenDisposition : uint8_t {
    Accepted = 0,
    Rejected,
    Failed
};

class [[nodiscard]] DecodeOpenResult {
public:
    [[nodiscard]] static constexpr DecodeOpenResult accepted(
        MediaProbe probe) noexcept {
        return probe.is_valid()
            ? DecodeOpenResult{DecodeOpenDisposition::Accepted, DecodeFailure::None, probe}
            : failed(DecodeFailure::BoundaryFailure);
    }

    [[nodiscard]] static constexpr DecodeOpenResult rejected(
        DecodeFailure failure) noexcept {
        return {
            DecodeOpenDisposition::Rejected,
            normalize_failure(failure),
            {}
        };
    }

    [[nodiscard]] static constexpr DecodeOpenResult failed(
        DecodeFailure failure) noexcept {
        return {
            DecodeOpenDisposition::Failed,
            normalize_failure(failure),
            {}
        };
    }

    [[nodiscard]] constexpr DecodeOpenDisposition disposition() const noexcept {
        return disposition_;
    }

    [[nodiscard]] constexpr DecodeFailure failure() const noexcept {
        return failure_;
    }

    [[nodiscard]] constexpr const MediaProbe& probe() const noexcept {
        return probe_;
    }

    [[nodiscard]] constexpr bool is_accepted() const noexcept {
        return disposition_ == DecodeOpenDisposition::Accepted;
    }

    [[nodiscard]] constexpr bool operator==(
        const DecodeOpenResult&) const noexcept = default;

private:
    constexpr DecodeOpenResult(
        DecodeOpenDisposition disposition,
        DecodeFailure failure,
        MediaProbe probe) noexcept
        : disposition_(disposition), failure_(failure), probe_(probe) {}

    [[nodiscard]] static constexpr DecodeFailure normalize_failure(
        DecodeFailure failure) noexcept {
        return failure == DecodeFailure::None
            ? DecodeFailure::BoundaryFailure
            : failure;
    }

    DecodeOpenDisposition disposition_;
    DecodeFailure failure_;
    MediaProbe probe_;
};

} // namespace kivo::core::decode
