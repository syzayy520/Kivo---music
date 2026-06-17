#include "metadata_repair_review_queue.hpp"

namespace kivo::metadata {

namespace {

QString issueTypeDisplayReason(MetadataIssueType type,
                               const QStringList& evidence,
                               const QStringList& rejectReasons) {
    switch (type) {
    case MetadataIssueType::NoIssue:
        return QStringLiteral("No issue detected");
    case MetadataIssueType::PossibleTitleArtistSwap:
        if (!evidence.isEmpty()) {
            return QStringLiteral("Possible title/artist swap: %1")
                .arg(evidence.join(QStringLiteral("; ")));
        }
        return QStringLiteral("Possible title/artist swap detected");
    case MetadataIssueType::MissingArtist:
        return QStringLiteral("Missing or placeholder artist");
    case MetadataIssueType::MissingTitle:
        return QStringLiteral("Missing title");
    case MetadataIssueType::InvalidMetadata:
        return QStringLiteral("Invalid metadata");
    case MetadataIssueType::LowConfidence:
        if (!rejectReasons.isEmpty()) {
            return QStringLiteral("Low confidence: %1")
                .arg(rejectReasons.join(QStringLiteral("; ")));
        }
        return QStringLiteral("Low confidence suggestion");
    }
    return QStringLiteral("Unknown issue");
}

bool filterAcceptsIssueType(const MetadataRepairReviewFilter& filter,
                            MetadataIssueType type) {
    switch (type) {
    case MetadataIssueType::NoIssue:
        return filter.includeNoIssue;
    case MetadataIssueType::PossibleTitleArtistSwap:
        return filter.includePossibleSwap;
    case MetadataIssueType::MissingArtist:
        return filter.includeMissingArtist;
    case MetadataIssueType::MissingTitle:
        return filter.includeMissingTitle;
    case MetadataIssueType::InvalidMetadata:
        return filter.includeInvalidMetadata;
    case MetadataIssueType::LowConfidence:
        // Low-confidence items are governed by minimumConfidence rather than a
        // dedicated include flag, so they pass the type check here and are later
        // filtered by confidence threshold.
        return true;
    }
    return false;
}

}  // namespace

MetadataRepairReviewItem MetadataRepairReviewItem::fromCandidate(
    const FieldSwapCandidate& candidate) {
    MetadataRepairReviewItem item;
    item.filePath = candidate.filePath;
    item.originalTitle = candidate.originalTitle;
    item.originalArtist = candidate.originalArtist;
    item.suggestedTitle = candidate.suggestedTitle;
    item.suggestedArtist = candidate.suggestedArtist;
    item.issueType = candidate.issueType;
    item.confidence = candidate.confidence;
    item.requiresReview = candidate.requiresReview;
    item.autoApplyAllowed = candidate.autoApplyAllowed;
    item.evidenceList = candidate.evidenceList;
    item.rejectReasons = candidate.rejectReasons;
    item.displayReason = issueTypeDisplayReason(
        candidate.issueType, candidate.evidenceList, candidate.rejectReasons);
    return item;
}

MetadataRepairReviewQueue MetadataRepairReviewQueue::fromReport(
    const MetadataRepairReport& report,
    const MetadataRepairReviewFilter& filter) {
    MetadataRepairReviewQueue queue;

    QList<MetadataRepairReviewItem> filtered;
    filtered.reserve(report.candidates.size());

    for (const auto& candidate : report.candidates) {
        if (!filterAcceptsIssueType(filter, candidate.issueType)) {
            continue;
        }
        if (candidate.confidence < filter.minimumConfidence) {
            continue;
        }

        filtered << MetadataRepairReviewItem::fromCandidate(candidate);
    }

    // Sort by confidence descending so high-risk / high-confidence items appear
    // first. std::stable_sort preserves input order for equal confidence.
    std::stable_sort(filtered.begin(), filtered.end(),
                     [](const MetadataRepairReviewItem& a,
                        const MetadataRepairReviewItem& b) {
                         return a.confidence > b.confidence;
                     });

    queue.items = filtered;
    queue.totalItems = filtered.size();

    for (const auto& item : queue.items) {
        switch (item.issueType) {
        case MetadataIssueType::NoIssue:
            queue.noIssueCount++;
            break;
        case MetadataIssueType::PossibleTitleArtistSwap:
            queue.possibleSwapCount++;
            break;
        case MetadataIssueType::MissingArtist:
            queue.missingArtistCount++;
            break;
        case MetadataIssueType::MissingTitle:
            queue.missingTitleCount++;
            break;
        case MetadataIssueType::InvalidMetadata:
            queue.invalidMetadataCount++;
            break;
        case MetadataIssueType::LowConfidence:
            queue.lowConfidenceCount++;
            break;
        }

        if (item.requiresReview) {
            queue.requiresReviewCount++;
        }
        if (item.autoApplyAllowed) {
            queue.autoApplyAllowedCount++;
        }
    }

    return queue;
}

}  // namespace kivo::metadata
