#include "../../../fixture/decode_boundary_probe_factory.hpp"
#include "../../../fixture/decode_boundary_test_runner.hpp"

namespace {

using namespace kivo;

void media_probe_requires_complete_truth() {
    constexpr auto probe = decode_boundary::test::valid_probe();
    static_assert(probe.is_valid());
    DECODE_ASSERT(probe.is_valid());

    auto invalid = probe;
    invalid.resample.target_rate = 44100;
    DECODE_ASSERT(!invalid.is_valid());

    invalid = probe;
    invalid.codec = core::decode::AudioCodec::Unknown;
    DECODE_ASSERT(!invalid.is_valid());
}

} // namespace

void run_media_probe_contract_tests(DecodeBoundaryTestRunner& runner) {
    runner.run(
        "media_probe_requires_complete_truth",
        media_probe_requires_complete_truth);
}
