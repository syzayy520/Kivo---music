#include "../../../../fixture/decode_boundary_test_runner.hpp"
#include "../../../../fixture/local_binary_file_fixture.hpp"

#include "kivo/source/local/local_file_byte_source.hpp"

namespace {

using namespace kivo;

void invalid_read_and_seek_fail_explicitly() {
    decode_boundary::test::TemporaryBinaryFile file;
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

void run_local_file_failure_tests(DecodeBoundaryTestRunner& runner) {
    runner.run(
        "invalid_read_and_seek_fail_explicitly",
        invalid_read_and_seek_fail_explicitly);
}
