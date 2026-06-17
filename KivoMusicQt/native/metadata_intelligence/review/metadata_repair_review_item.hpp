#pragma once

#include "../repair/id3_field_swap/field_swap_candidate.hpp"

#include <QString>
#include <QStringList>

namespace kivo::metadata {

// =============================================================================
// MetadataRepairReviewItem
// 职责：承载 UI/人工复核可展示的单条 review item
// 单文件单职责：只放数据，不写数据库、不写 tag、不执行修复
// =============================================================================
struct MetadataRepairReviewItem {
    // Original values from the source file
    QString filePath;
    QString originalTitle;
    QString originalArtist;

    // Suggested corrected values (same as original if no swap is suggested)
    QString suggestedTitle;
    QString suggestedArtist;

    // Issue classification for review
    MetadataIssueType issueType = MetadataIssueType::NoIssue;

    // Confidence score (0.0 to 1.0), copied from the candidate
    double confidence = 0.0;

    // Safety flags: display/ statistics only; no auto-apply is triggered here
    bool requiresReview = true;
    bool autoApplyAllowed = false;

    // Evidence supporting the issue (or swap hypothesis)
    QStringList evidenceList;

    // Reasons why a suggestion was rejected or downgraded
    QStringList rejectReasons;

    // Human-readable reason intended for UI/CLI display
    QString displayReason;

    // Build a review item from a formalized candidate. The candidate is never
    // modified; this is a read-only projection for review purposes.
    static MetadataRepairReviewItem fromCandidate(const FieldSwapCandidate& candidate);
};

}  // namespace kivo::metadata
