#pragma once

#include <optional>

#include "kivo/core/decode/source/byte_source_boundary.hpp"
#include "kivo/host/abi/source/kivo_audio_source.h"

namespace kivo::host::abi {

class HostCallbackByteSource final
    : public core::decode::ByteSourceBoundary {
public:
    explicit HostCallbackByteSource(
        const kivo_audio_source_v1& source) noexcept;
    ~HostCallbackByteSource() override;

    HostCallbackByteSource(const HostCallbackByteSource&) = delete;
    HostCallbackByteSource& operator=(const HostCallbackByteSource&) = delete;

    [[nodiscard]] core::contract::SourceIdentity identity() const noexcept override;
    [[nodiscard]] core::contract::SourceGeneration generation() const noexcept override;
    [[nodiscard]] bool seekable() const noexcept override;
    [[nodiscard]] std::optional<uint64_t> size_bytes() const noexcept override;
    [[nodiscard]] core::decode::ByteSourceReadResult read(
        std::span<std::byte> destination) noexcept override;
    [[nodiscard]] core::decode::ByteSourceSeekResult seek(
        int64_t offset,
        core::decode::ByteSeekOrigin origin) noexcept override;

private:
    kivo_audio_source_v1 source_{};
};

} // namespace kivo::host::abi
