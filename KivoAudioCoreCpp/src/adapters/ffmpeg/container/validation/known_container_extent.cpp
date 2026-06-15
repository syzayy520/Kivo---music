#include "known_container_extent.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace kivo::adapters::ffmpeg::detail {

namespace {

[[nodiscard]] constexpr bool matches(
    const std::array<std::byte, 12>& bytes,
    size_t offset,
    const char (&text)[5]) noexcept {
    for (size_t index = 0; index < 4; ++index) {
        if (bytes[offset + index] != std::byte{
                static_cast<unsigned char>(text[index])}) {
            return false;
        }
    }
    return true;
}

[[nodiscard]] constexpr uint32_t little_endian_u32(
    const std::array<std::byte, 12>& bytes,
    size_t offset) noexcept {
    return static_cast<uint32_t>(bytes[offset])
        | static_cast<uint32_t>(bytes[offset + 1]) << 8u
        | static_cast<uint32_t>(bytes[offset + 2]) << 16u
        | static_cast<uint32_t>(bytes[offset + 3]) << 24u;
}

} // namespace

core::decode::DecodeFailure validate_known_container_extent(
    core::decode::ByteSourceBoundary& source) noexcept {
    using namespace core::decode;

    const auto size = source.size_bytes();
    if (!source.seekable() || !size.has_value()) {
        return DecodeFailure::None;
    }

    std::array<std::byte, 12> header{};
    const auto read = source.read(header);
    const auto reset = source.seek(0, ByteSeekOrigin::Begin);
    if (reset.disposition != ByteSourceSeekDisposition::Succeeded) {
        return DecodeFailure::SourceSeekFailed;
    }
    if (read.disposition == ByteSourceReadDisposition::Failed) {
        return read.failure;
    }
    if (read.bytes_read != header.size()) {
        return DecodeFailure::None;
    }

    if (matches(header, 0, "RIFF") && matches(header, 8, "WAVE")) {
        const auto declared_size =
            static_cast<uint64_t>(little_endian_u32(header, 4)) + 8u;
        if (declared_size > *size) {
            return DecodeFailure::InvalidMediaData;
        }
    }
    return DecodeFailure::None;
}

} // namespace kivo::adapters::ffmpeg::detail
