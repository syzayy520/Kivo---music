#include "../../../../fixture/decode_boundary_test_runner.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <system_error>

#include "kivo/source/local/local_file_byte_source.hpp"

namespace {

using namespace kivo;

class TemporaryLargeSparseFile {
public:
    TemporaryLargeSparseFile() {
        path = std::filesystem::temp_directory_path()
            / "kivo_large_byte_source_test.bin";
        {
            std::ofstream create(path, std::ios::binary | std::ios::trunc);
            if (!create) {
                throw std::runtime_error(
                    "Failed to create large-file fixture");
            }
        }
        std::error_code error;
        std::filesystem::resize_file(path, size, error);
        if (error) {
            throw std::runtime_error(
                "Failed to create sparse large-file fixture");
        }
        std::fstream stream(
            path,
            std::ios::binary | std::ios::in | std::ios::out);
        stream.seekp(static_cast<std::streamoff>(size - marker.size()));
        stream.write(
            reinterpret_cast<const char*>(marker.data()),
            static_cast<std::streamsize>(marker.size()));
        if (!stream) {
            throw std::runtime_error(
                "Failed to write sparse large-file marker");
        }
    }

    ~TemporaryLargeSparseFile() {
        std::error_code error;
        std::filesystem::remove(path, error);
    }

    static constexpr uint64_t size = (uint64_t{4} << 30u) + 8192u;
    static constexpr std::array<std::byte, 4> marker{
        std::byte{0x4b},
        std::byte{0x49},
        std::byte{0x56},
        std::byte{0x4f}
    };
    std::filesystem::path path;
};

void source_size_and_seek_are_64_bit() {
    TemporaryLargeSparseFile file;
    auto source =
        source::local::LocalFileByteSource::open(file.path, {401}, {});
    DECODE_ASSERT(source);
    DECODE_ASSERT(source->size_bytes() == file.size);

    const auto seek = source->seek(
        static_cast<int64_t>(file.size - file.marker.size()),
        core::decode::ByteSeekOrigin::Begin);
    DECODE_ASSERT(
        seek.disposition
        == core::decode::ByteSourceSeekDisposition::Succeeded);
    DECODE_ASSERT(seek.position == file.size - file.marker.size());

    std::array<std::byte, 4> bytes{};
    const auto read = source->read(bytes);
    DECODE_ASSERT(read.bytes_read == bytes.size());
    DECODE_ASSERT(bytes == file.marker);
    DECODE_ASSERT(
        source->read(bytes).disposition
        == core::decode::ByteSourceReadDisposition::EndOfStream);
}

} // namespace

void run_large_file_byte_source_tests(DecodeBoundaryTestRunner& runner) {
    runner.run(
        "source_size_and_seek_are_64_bit",
        source_size_and_seek_are_64_bit);
}
