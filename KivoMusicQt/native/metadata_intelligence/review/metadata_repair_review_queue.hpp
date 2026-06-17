#pragma once

#include "metadata_repair_review_filter.hpp"
#include "metadata_repair_review_item.hpp"
#include "../report/metadata_repair_report.hpp"

#include <QList>

namespace kivo::metadata {

// =============================================================================
// MetadataRepairReviewQueue
// 职责：从 MetadataRepairReport 生成 UI/人工复核可消费的 review queue
// 单文件单职责：只负责过滤、排序、投影；不修改原 report/candidate，不执行修复
// 禁止：写数据库、写源文件 tag、调用 applyMusicRecords、访问 MusicFileRecord 扫描流程
// =============================================================================
struct MetadataRepairReviewQueue {
    QList<MetadataRepairReviewItem> items;

    int totalItems = 0;
    int noIssueCount = 0;
    int missingArtistCount = 0;
    int missingTitleCount = 0;
    int possibleSwapCount = 0;
    int invalidMetadataCount = 0;
    int lowConfidenceCount = 0;
    int requiresReviewCount = 0;
    int autoApplyAllowedCount = 0;  // Display/statistics only; no auto-apply here

    // Build a review queue from a read-only report. The report and its
    // candidates are never modified. Default filter excludes NoIssue and keeps
    // MissingArtist / MissingTitle / PossibleTitleArtistSwap / InvalidMetadata.
    static MetadataRepairReviewQueue fromReport(
        const MetadataRepairReport& report,
        const MetadataRepairReviewFilter& filter = MetadataRepairReviewFilter());
};

}  // namespace kivo::metadata
