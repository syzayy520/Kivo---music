#pragma once

namespace kivo::metadata {

// =============================================================================
// MetadataRepairReviewFilter
// 职责：定义 review queue 的过滤选项
// 单文件单职责：只放结构，不放执行逻辑
// =============================================================================
struct MetadataRepairReviewFilter {
    bool includeNoIssue = false;
    bool includeMissingArtist = true;
    bool includeMissingTitle = true;
    bool includePossibleSwap = true;
    bool includeInvalidMetadata = true;

    // Minimum confidence threshold (0.0 to 1.0). Items with a lower confidence
    // are excluded from the queue.
    double minimumConfidence = 0.0;
};

}  // namespace kivo::metadata
