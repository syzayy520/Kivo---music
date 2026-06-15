#include "../../../../fixture/decode_boundary_test_runner.hpp"
#include "../../../../fixture/local_binary_file_fixture.hpp"

#include "kivo/source/local/local_file_byte_source.hpp"

namespace {

using namespace kivo;

void open_rejects_invalid_path_and_identity() {
    DECODE_ASSERT(!source::local::LocalFileByteSource::open({}, {1}, {}));
    DECODE_ASSERT(!source::local::LocalFileByteSource::open(
        std::filesystem::temp_directory_path() / "kivo_missing_source.bin",
        {1},
        {}));

    decode_boundary::test::TemporaryBinaryFile file;
    DECODE_ASSERT(!source::local::LocalFileByteSource::open(file.path, {}, {}));
}

} // namespace

void run_local_file_open_tests(DecodeBoundaryTestRunner& runner) {
    runner.run(
        "open_rejects_invalid_path_and_identity",
        open_rejects_invalid_path_and_identity);
}
