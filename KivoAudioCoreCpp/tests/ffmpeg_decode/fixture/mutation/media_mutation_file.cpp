#include "media_mutation_file.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <system_error>

namespace ffmpeg_decode_test {

namespace {

[[nodiscard]] std::filesystem::path mutation_path(
    const std::filesystem::path& source,
    const char* tag) {
    return source.parent_path()
        / (source.stem().string() + "_" + tag + source.extension().string());
}

void copy_prefix(
    const std::filesystem::path& source_path,
    const std::filesystem::path& destination_path,
    uintmax_t byte_count) {
    std::ifstream source(source_path, std::ios::binary);
    std::ofstream destination(
        destination_path,
        std::ios::binary | std::ios::trunc);
    if (!source || !destination) {
        throw std::runtime_error("Failed to open media mutation file");
    }

    std::array<char, 65536> buffer{};
    while (byte_count != 0) {
        const auto requested = static_cast<std::streamsize>(
            std::min<uintmax_t>(byte_count, buffer.size()));
        source.read(buffer.data(), requested);
        const auto read = source.gcount();
        if (read <= 0) {
            throw std::runtime_error("Failed to read media mutation source");
        }
        destination.write(buffer.data(), read);
        if (!destination) {
            throw std::runtime_error("Failed to write media mutation");
        }
        byte_count -= static_cast<uintmax_t>(read);
    }
}

void zero_prefix(const std::filesystem::path& path, uintmax_t byte_count) {
    std::fstream stream(
        path,
        std::ios::binary | std::ios::in | std::ios::out);
    if (!stream) {
        throw std::runtime_error("Failed to reopen media mutation");
    }
    const std::array<char, 4096> zeros{};
    while (byte_count != 0) {
        const auto count = static_cast<std::streamsize>(
            std::min<uintmax_t>(byte_count, zeros.size()));
        stream.write(zeros.data(), count);
        if (!stream) {
            throw std::runtime_error("Failed to zero media prefix");
        }
        byte_count -= static_cast<uintmax_t>(count);
    }
}

void append_junk(const std::filesystem::path& path, uintmax_t byte_count) {
    std::ofstream stream(path, std::ios::binary | std::ios::app);
    if (!stream) {
        throw std::runtime_error("Failed to append media mutation");
    }
    std::array<uint8_t, 4096> bytes{};
    for (size_t index = 0; index < bytes.size(); ++index) {
        bytes[index] = static_cast<uint8_t>((index * 131u + 0x4bu) & 0xffu);
    }
    while (byte_count != 0) {
        const auto count = static_cast<std::streamsize>(
            std::min<uintmax_t>(byte_count, bytes.size()));
        stream.write(
            reinterpret_cast<const char*>(bytes.data()),
            count);
        if (!stream) {
            throw std::runtime_error("Failed to append deterministic junk");
        }
        byte_count -= static_cast<uintmax_t>(count);
    }
}

} // namespace

MediaMutationFile::~MediaMutationFile() {
    std::error_code error;
    std::filesystem::remove(path_, error);
}

MediaMutationFile MediaMutationFile::truncated(
    const std::filesystem::path& source,
    uintmax_t numerator,
    uintmax_t denominator,
    const char* tag) {
    if (numerator == 0 || denominator == 0 || numerator >= denominator) {
        throw std::runtime_error("Invalid media truncation fraction");
    }
    return MediaMutationFile{
        source,
        tag,
        Operation::Truncate,
        numerator,
        denominator};
}

MediaMutationFile MediaMutationFile::zeroed_prefix(
    const std::filesystem::path& source,
    size_t byte_count,
    const char* tag) {
    return MediaMutationFile{
        source,
        tag,
        Operation::ZeroPrefix,
        byte_count,
        0};
}

MediaMutationFile MediaMutationFile::appended_junk(
    const std::filesystem::path& source,
    size_t byte_count,
    const char* tag) {
    return MediaMutationFile{
        source,
        tag,
        Operation::AppendJunk,
        byte_count,
        0};
}

const std::filesystem::path& MediaMutationFile::path() const noexcept {
    return path_;
}

MediaMutationFile::MediaMutationFile(
    const std::filesystem::path& source,
    const char* tag,
    Operation operation,
    uintmax_t first,
    uintmax_t second)
    : path_(mutation_path(source, tag)) {
    const auto source_size = std::filesystem::file_size(source);
    const auto copy_size = operation == Operation::Truncate
        ? source_size / second * first
            + source_size % second * first / second
        : source_size;
    copy_prefix(source, path_, copy_size);

    if (operation == Operation::ZeroPrefix) {
        zero_prefix(path_, std::min<uintmax_t>(first, source_size));
    } else if (operation == Operation::AppendJunk) {
        append_junk(path_, first);
    }
}

} // namespace ffmpeg_decode_test
