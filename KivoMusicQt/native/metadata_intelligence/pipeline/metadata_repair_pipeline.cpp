#include "metadata_repair_pipeline.hpp"

#include "../confidence/swap_detector.hpp"
#include "../repair/swap_corrector.hpp"

namespace kivo::metadata {

MetadataRepairReport MetadataRepairPipeline::run(const QList<MusicFileRecord>& records) {
    // Step 1: detect candidates (read-only, no side effects).
    const QList<FieldSwapCandidate> detected = FieldSwapDetector::detectBatch(records);

    // Step 2: formalize safety checks (read-only, produces refined suggestions).
    const QList<FieldSwapCandidate> formalized = FieldSwapCorrector::formalizeBatch(detected);

    // Step 3: build report-only aggregation.
    return MetadataRepairReport::fromCandidates(formalized, records.size());
}

}  // namespace kivo::metadata