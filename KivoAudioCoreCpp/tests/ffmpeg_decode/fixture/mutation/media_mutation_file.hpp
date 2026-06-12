#pragma once

#include <cstddef>
#include <filesystem>

namespace ffmpeg_decode_test {

class MediaMutationFile final {
public:
    ~MediaMutationFile();

    MediaMutationFile(const MediaMutationFile&) = delete;
    MediaMutationFile& operator=(const MediaMutationFile&) = delete;
    MediaMutationFile(MediaMutationFile&&) = delete;
    MediaMutationFile& operator=(MediaMutationFile&&) = delete;

    [[nodiscard]] static MediaMutationFile truncated(
        const std::filesystem::path& source,
        uintmax_t numerator,
        uintmax_t denominator,
        const char* tag);
    [[nodiscard]] static MediaMutationFile zeroed_prefix(
        const std::filesystem::path& source,
        size_t byte_count,
        const char* tag);
    [[nodiscard]] static MediaMutationFile appended_junk(
        const std::filesystem::path& source,
        size_t byte_count,
        const char* tag);

    [[nodiscard]] const std::filesystem::path& path() const noexcept;

private:
    enum class Operation {
        Truncate,
        ZeroPrefix,
        AppendJunk
    };

    MediaMutationFile(
        const std::filesystem::path& source,
        const char* tag,
        Operation operation,
        uintmax_t first,
        uintmax_t second);

    std::filesystem::path path_;
};

} // namespace ffmpeg_decode_test
