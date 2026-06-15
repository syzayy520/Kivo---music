#pragma once

#include <filesystem>
#include <memory>

#include "kivo/core/decode/source/byte_source_boundary.hpp"

namespace kivo::source::local {

class LocalFileByteSource final : public core::decode::ByteSourceBoundary {
public:
    [[nodiscard]] static std::unique_ptr<LocalFileByteSource> open(
        const std::filesystem::path& path,
        core::contract::SourceIdentity identity,
        core::contract::SourceGeneration generation) noexcept;

    ~LocalFileByteSource() override;

    LocalFileByteSource(const LocalFileByteSource&) = delete;
    LocalFileByteSource& operator=(const LocalFileByteSource&) = delete;
    LocalFileByteSource(LocalFileByteSource&&) = delete;
    LocalFileByteSource& operator=(LocalFileByteSource&&) = delete;

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
    class Impl;

    explicit LocalFileByteSource(std::unique_ptr<Impl> impl) noexcept;

    std::unique_ptr<Impl> impl_;
};

} // namespace kivo::source::local
