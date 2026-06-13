#include "../../../../fixture/decode_boundary_test_runner.hpp"
#include "../../../../fixture/local_binary_file_fixture.hpp"

#include "kivo/source/local/local_file_byte_source.hpp"

#include <array>
#include <cstddef>

namespace {

using namespace kivo;

void seek_supports_begin_current_and_end() {
    decode_boundary::test::TemporaryBinaryFile file;
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

} // namespace

void run_local_file_seek_tests(DecodeBoundaryTestRunner& runner) {
    runner.run(
        "seek_supports_begin_current_and_end",
        seek_supports_begin_current_and_end);
}
