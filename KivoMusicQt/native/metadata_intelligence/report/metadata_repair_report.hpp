#pragma once

#include "metadata_repair_issue_summary.hpp"

#include "../repair/id3_field_swap/field_swap_candidate.hpp"

#include <QList>
#include <QString>

namespace kivo::metadata {

// =============================================================================
// MetadataRepairReport
// 职责：将 detector + corrector 生成的候选列表转换为只读报告
// 单文件单职责：聚合统计、生成分类汇总，不修改输入、不写库、不写文件
// =============================================================================
struct MetadataRepairReport {
    int totalRecords = 0;

    int noIssueCount = 0;
    int possibleSwapCount = 0;
    int missingArtistCount = 0;
    int missingTitleCount = 0;
    int invalidMetadataCount = 0;
    int lowConfidenceCount = 0;
    int repairCandidateCount = 0;  // isRepairCandidate() == true
    int suggestSwapCount = 0;      // shouldSuggestSwap() == true
    int autoApplyAllowedCount = 0; // autoApplyAllowed == true (本阶段恒为 0)

    QList<FieldSwapCandidate> candidates;
    QList<MetadataRepairIssueSummary> summaries;

    // Build a report from formalized candidates. The input list must already be
    // processed by FieldSwapCorrector::formalizeBatch().
    static MetadataRepairReport fromCandidates(const QList<FieldSwapCandidate>& candidates,
                                               int totalRecords);

    // Human-readable one-line summary for UI/CLI display.
    QString summaryLine() const;
};

}  // namespace kivo::metadata