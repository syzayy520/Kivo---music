#include "../../../fixture/decode_boundary_format_factory.hpp"
#include "../../../fixture/decode_boundary_test_runner.hpp"

#include "kivo/core/decode/block/decoded_audio_block_view.hpp"

#include <array>
#include <cstddef>

namespace {

using namespace kivo;

void decoded_block_requires_exact_frame_bytes() {
    const std::array<std::byte, 32> bytes{};
    const core::decode::DecodedAudioBlockView valid{
        bytes,
        decode_boundary::test::output_format(),
        4,
        100,
        {},
        {},
        true
    };
    DECODE_ASSERT(valid.is_valid());

    auto invalid = valid;
    invalid.frame_count = 5;
    DECODE_ASSERT(!invalid.is_valid());
}

} // namespace

void run_decoded_block_contract_tests(DecodeBoundaryTestRunner& runner) {
    runner.run(
        "decoded_block_requires_exact_frame_bytes",
        decoded_block_requires_exact_frame_bytes);
}
