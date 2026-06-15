#include "../../../../fixture/decode_boundary_test_runner.hpp"
#include "../../../../fixture/local_binary_file_fixture.hpp"

#include "kivo/source/local/local_file_byte_source.hpp"

#include <array>
#include <cstddef>

namespace {

using namespace kivo;

void read_size_and_eos_are_exact() {
    decode_boundary::test::TemporaryBinaryFile file;
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

} // namespace

void run_local_file_read_tests(DecodeBoundaryTestRunner& runner) {
    runner.run("read_size_and_eos_are_exact", read_size_and_eos_are_exact);
}
