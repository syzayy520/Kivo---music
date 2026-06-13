#include "../../fixture/observability_test_assert.hpp"
#include "../../runner/observability_test_cases.hpp"

#include "kivo/core/observability/category/decode_failure_category.hpp"

namespace observability_test {

void decode_failures_have_stable_categories() {
    using kivo::core::decode::DecodeFailure;
    using kivo::core::observability::DecodeFailureCategory;
    using kivo::core::observability::classify_decode_failure;

    OBSERVABILITY_ASSERT(
        classify_decode_failure(DecodeFailure::SourceReadFailed)
        == DecodeFailureCategory::Source);
    OBSERVABILITY_ASSERT(
        classify_decode_failure(DecodeFailure::UnsupportedContainer)
        == DecodeFailureCategory::Container);
    OBSERVABILITY_ASSERT(
        classify_decode_failure(DecodeFailure::UnsupportedCodec)
        == DecodeFailureCategory::Codec);
    OBSERVABILITY_ASSERT(
        classify_decode_failure(DecodeFailure::InvalidMediaData)
        == DecodeFailureCategory::MediaData);
    OBSERVABILITY_ASSERT(
        classify_decode_failure(DecodeFailure::ConversionFailed)
        == DecodeFailureCategory::Conversion);
    OBSERVABILITY_ASSERT(
        classify_decode_failure(DecodeFailure::ProcessingFailed)
        == DecodeFailureCategory::Processing);
    OBSERVABILITY_ASSERT(
        classify_decode_failure(DecodeFailure::StaleDecodeGeneration)
        == DecodeFailureCategory::Generation);
    OBSERVABILITY_ASSERT(
        classify_decode_failure(DecodeFailure::BoundaryFailure)
        == DecodeFailureCategory::Boundary);
}

} // namespace observability_test
