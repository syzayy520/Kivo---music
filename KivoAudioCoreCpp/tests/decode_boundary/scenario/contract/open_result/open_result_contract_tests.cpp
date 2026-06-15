#include "../../../fixture/decode_boundary_probe_factory.hpp"
#include "../../../fixture/decode_boundary_test_runner.hpp"

#include "kivo/core/decode/result/decode_open_result.hpp"

namespace {

using namespace kivo;

void open_result_rejects_invalid_probe() {
    const auto accepted =
        core::decode::DecodeOpenResult::accepted(decode_boundary::test::valid_probe());
    DECODE_ASSERT(accepted.is_accepted());
    DECODE_ASSERT(accepted.failure() == core::decode::DecodeFailure::None);

    const auto invalid = core::decode::DecodeOpenResult::accepted({});
    DECODE_ASSERT(!invalid.is_accepted());
    DECODE_ASSERT(invalid.failure() == core::decode::DecodeFailure::BoundaryFailure);
}

} // namespace

void run_decode_open_result_contract_tests(DecodeBoundaryTestRunner& runner) {
    runner.run("open_result_rejects_invalid_probe", open_result_rejects_invalid_probe);
}
