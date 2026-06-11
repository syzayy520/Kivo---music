#include "decode_boundary_tests_main.hpp"

#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>

#include "kivo/source/local/local_file_byte_source.hpp"

namespace {

using namespace kivo;

class TemporaryBinaryFile {
public:
    TemporaryBinaryFile() {
        path = std::filesystem::temp_directory_path() / "kivo_byte_source_test.bin";
        std::ofstream output(path, std::ios::binary | std::ios::trunc);
        const std::array<unsigned char, 10> bytes{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        output.write(
            reinterpret_cast<const char*>(bytes.data()),
            static_cast<std::streamsize>(bytes.size()));
    }

    ~TemporaryBinaryFile() {
        std::error_code error;
        std::filesystem::remove(path, error);
    }

    std::filesystem::path path;
};

void open_rejects_invalid_path_and_identity() {
    DECODE_ASSERT(!source::local::LocalFileByteSource::open(
        {},
        {1},
        {}));
    DECODE_ASSERT(!source::local::LocalFileByteSource::open(
        std::filesystem::temp_directory_path() / "kivo_missing_source.bin",
        {1},
        {}));
    TemporaryBinaryFile file;
    DECODE_ASSERT(!source::local::LocalFileByteSource::open(file.path, {}, {}));
}

void read_size_and_eos_are_exact() {
    TemporaryBinaryFile file;
    auto source = source::local::LocalFileByteSource::open(file.path, {7}, {});
    DECODE_ASSERT(source);
    DECODE_ASSERT(source->identity().value == 7);
    DECODE_ASSERT(source->seekable());
    DECODE_ASSERT(source->size_bytes() == 10);

    std::array<std::byte, 6> first{};
    const auto first_result = source->read(first);
    DECODE_ASSERT(
        first_result.disposition
        == core::decode::ByteSourceReadDisposition::Data);
    DECODE_ASSERT(first_result.bytes_read == 6);
    DECODE_ASSERT(first[5] == std::byte{5});

    std::array<std::byte, 6> second{};
    const auto second_result = source->read(second);
    DECODE_ASSERT(second_result.bytes_read == 4);
    DECODE_ASSERT(second[3] == std::byte{9});

    const auto eos = source->read(second);
    DECODE_ASSERT(
        eos.disposition == core::decode::ByteSourceReadDisposition::EndOfStream);
    DECODE_ASSERT(eos.failure == core::decode::DecodeFailure::EndOfStream);
}

void seek_supports_begin_current_and_end() {
    TemporaryBinaryFile file;
    auto source = source::local::LocalFileByteSource::open(file.path, {8}, {});
    DECODE_ASSERT(source);

    auto seek = source->seek(4, core::decode::ByteSeekOrigin::Begin);
    DECODE_ASSERT(
        seek.disposition == core::decode::ByteSourceSeekDisposition::Succeeded);
    DECODE_ASSERT(seek.position == 4);

    std::array<std::byte, 2> bytes{};
    DECODE_ASSERT(source->read(bytes).bytes_read == 2);
    DECODE_ASSERT(bytes[0] == std::byte{4});

    seek = source->seek(-1, core::decode::ByteSeekOrigin::Current);
    DECODE_ASSERT(seek.position == 5);
    DECODE_ASSERT(source->read(bytes).bytes_read == 2);
    DECODE_ASSERT(bytes[0] == std::byte{5});

    seek = source->seek(-2, core::decode::ByteSeekOrigin::End);
    DECODE_ASSERT(seek.position == 8);
    DECODE_ASSERT(source->read(bytes).bytes_read == 2);
    DECODE_ASSERT(bytes[1] == std::byte{9});
}

void invalid_read_and_seek_fail_explicitly() {
    TemporaryBinaryFile file;
    auto source = source::local::LocalFileByteSource::open(file.path, {9}, {});
    DECODE_ASSERT(source);
    DECODE_ASSERT(
        source->read({}).failure == core::decode::DecodeFailure::InvalidBuffer);
    const auto seek = source->seek(-1, core::decode::ByteSeekOrigin::Begin);
    DECODE_ASSERT(
        seek.disposition == core::decode::ByteSourceSeekDisposition::Failed);
    DECODE_ASSERT(seek.failure == core::decode::DecodeFailure::SourceSeekFailed);
}

} // namespace

void run_local_file_byte_source_tests(DecodeBoundaryTestRunner& runner) {
    runner.run(
        "open_rejects_invalid_path_and_identity",
        open_rejects_invalid_path_and_identity);
    runner.run("read_size_and_eos_are_exact", read_size_and_eos_are_exact);
    runner.run(
        "seek_supports_begin_current_and_end",
        seek_supports_begin_current_and_end);
    runner.run(
        "invalid_read_and_seek_fail_explicitly",
        invalid_read_and_seek_fail_explicitly);
}
