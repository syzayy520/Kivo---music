#include "../../../fixture/decode_boundary_test_runner.hpp"

#include "kivo/core/decode/result/decode_control_result.hpp"

namespace {

using namespace kivo;

void unsupported_seek_and_flush_are_explicit() {
    const auto seek = core::decode::DecodeControlResult::unsupported(
        core::decode::DecodeFailure::SeekUnsupported);
    const auto flush = core::decode::DecodeControlResult::unsupported(
        core::decode::DecodeFailure::FlushUnsupported);
    DECODE_ASSERT(
        seek.disposition == core::decode::DecodeControlDisposition::Unsupported);
    DECODE_ASSERT(seek.failure == core::decode::DecodeFailure::SeekUnsupported);
    DECODE_ASSERT(flush.failure == core::decode::DecodeFailure::FlushUnsupported);
}

} // namespace

void run_decode_control_result_contract_tests(DecodeBoundaryTestRunner& runner) {
    runner.run(
        "unsupported_seek_and_flush_are_explicit",
        unsupported_seek_and_flush_are_explicit);
}
