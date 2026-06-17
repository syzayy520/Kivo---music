#pragma once

#include "../repair/id3_field_swap/field_swap_candidate.hpp"

#include <QStringList>

namespace kivo::metadata {

// =============================================================================
// MetadataRepairIssueSummary
// 职责：汇总某一类元数据问题的数量与代表性文件路径
// 单文件单职责：只承载计数数据，不做决策或写操作
// =============================================================================
struct MetadataRepairIssueSummary {
    MetadataIssueType issueType = MetadataIssueType::NoIssue;
    int count = 0;
    QStringList samplePaths;  // 最多保留 3 个示例路径，用于展示

    bool isEmpty() const { return count == 0; }
};

}  // namespace kivo::metadata