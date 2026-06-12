#pragma once

#include <optional>
#include <span>

#include "kivo/core/decode/source/byte_source_boundary.hpp"

namespace playback_runtime_test {

class TestByteSource final
    : public kivo::core::decode::ByteSourceBoundary {
public:
    TestByteSource(
        kivo::core::contract::SourceIdentity identity,
        kivo::core::contract::SourceGeneration generation) noexcept
        : identity_(identity), generation_(generation) {}

    [[nodiscard]] kivo::core::contract::SourceIdentity identity()
        const noexcept override {
        return identity_;
    }

    [[nodiscard]] kivo::core::contract::SourceGeneration generation()
        const noexcept override {
        return generation_;
    }

    [[nodiscard]] bool seekable() const noexcept override {
        return true;
    }

    [[nodiscard]] std::optional<uint64_t> size_bytes()
        const noexcept override {
        return 0;
    }

    [[nodiscard]] kivo::core::decode::ByteSourceReadResult read(
        std::span<std::byte>) noexcept override {
        return {
            kivo::core::decode::ByteSourceReadDisposition::EndOfStream,
            0,
            kivo::core::decode::DecodeFailure::EndOfStream
        };
    }

    [[nodiscard]] kivo::core::decode::ByteSourceSeekResult seek(
        int64_t,
        kivo::core::decode::ByteSeekOrigin) noexcept override {
        return {
            kivo::core::decode::ByteSourceSeekDisposition::Succeeded,
            0,
            kivo::core::decode::DecodeFailure::None
        };
    }

private:
    kivo::core::contract::SourceIdentity identity_{};
    kivo::core::contract::SourceGeneration generation_{};
};

} // namespace playback_runtime_test
