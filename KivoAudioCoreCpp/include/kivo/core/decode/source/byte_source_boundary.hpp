#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>

#include "kivo/core/contract/source/identity/source_generation.hpp"
#include "kivo/core/contract/source/identity/source_identity.hpp"
#include "kivo/core/decode/source/byte_source_result.hpp"

namespace kivo::core::decode {

class ByteSourceBoundary {
public:
    virtual ~ByteSourceBoundary() = default;

    [[nodiscard]] virtual contract::SourceIdentity identity() const noexcept = 0;
    [[nodiscard]] virtual contract::SourceGeneration generation() const noexcept = 0;
    [[nodiscard]] virtual bool seekable() const noexcept = 0;
    [[nodiscard]] virtual std::optional<uint64_t> size_bytes() const noexcept = 0;
    [[nodiscard]] virtual ByteSourceReadResult read(
        std::span<std::byte> destination) noexcept = 0;
    [[nodiscard]] virtual ByteSourceSeekResult seek(
        int64_t offset,
        ByteSeekOrigin origin) noexcept = 0;
};

} // namespace kivo::core::decode
