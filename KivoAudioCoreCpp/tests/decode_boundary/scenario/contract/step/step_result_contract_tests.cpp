#include "../../../fixture/decode_boundary_format_factory.hpp"
#include "../../../fixture/decode_boundary_test_runner.hpp"

#include "kivo/core/decode/result/decode_step_result.hpp"

#include <array>
#include <cstddef>

namespace {

using namespace kivo;

void step_result_keeps_eos_distinct_from_failure() {
    const std::array<std::byte, 32> bytes{};
    const core::decode::DecodedAudioBlockView block{
        bytes,
        decode_boundary::test::output_format(),
        4,
        0,
        {},
        {},
        false
    };
    const auto produced = core::decode::DecodeStepResult::produced(block);
    DECODE_ASSERT(
        produced.disposition() == core::decode::DecodeStepDisposition::Produced);
    DECODE_ASSERT(produced.failure() == core::decode::DecodeFailure::None);

    const auto eos = core::decode::DecodeStepResult::end_of_stream();
    DECODE_ASSERT(
        eos.disposition() == core::decode::DecodeStepDisposition::EndOfStream);
    DECODE_ASSERT(eos.failure() == core::decode::DecodeFailure::EndOfStream);

    const auto failed = core::decode::DecodeStepResult::failed(
        core::decode::DecodeFailure::SourceReadFailed);
    DECODE_ASSERT(
        failed.disposition() == core::decode::DecodeStepDisposition::Failed);
}

} // namespace

void run_decode_step_result_contract_tests(DecodeBoundaryTestRunner& runner) {
    runner.run(
        "step_result_keeps_eos_distinct_from_failure",
        step_result_keeps_eos_distinct_from_failure);
}
