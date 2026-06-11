#pragma once

#include <cstddef>
#include <cstdint>

#include "kivo/core/decode/failure/decode_failure.hpp"

namespace kivo::core::decode {

enum class ByteSourceReadDisposition : uint8_t {
    Data = 0,
    EndOfStream,
    Failed
};

struct ByteSourceReadResult {
    ByteSourceReadDisposition disposition{ByteSourceReadDisposition::Failed};
    size_t bytes_read{0};
    DecodeFailure failure{DecodeFailure::BoundaryFailure};

    [[nodiscard]] constexpr bool operator==(
        const ByteSourceReadResult&) const noexcept = default;
};

enum class ByteSeekOrigin : uint8_t {
    Begin = 0,
    Current,
    End
};

enum class ByteSourceSeekDisposition : uint8_t {
    Succeeded = 0,
    Unsupported,
    Failed
};

struct ByteSourceSeekResult {
    ByteSourceSeekDisposition disposition{ByteSourceSeekDisposition::Failed};
    uint64_t position{0};
    DecodeFailure failure{DecodeFailure::BoundaryFailure};

    [[nodiscard]] constexpr bool operator==(
        const ByteSourceSeekResult&) const noexcept = default;
};

} // namespace kivo::core::decode
