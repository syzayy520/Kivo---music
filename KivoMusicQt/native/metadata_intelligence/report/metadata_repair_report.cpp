#include "metadata_repair_report.hpp"

#include <QHash>

namespace kivo::metadata {

namespace {

const int kMaxSamplesPerSummary = 3;

QString issueTypeLabel(MetadataIssueType type) {
    switch (type) {
    case MetadataIssueType::NoIssue:
        return QStringLiteral("No issue");
    case MetadataIssueType::PossibleTitleArtistSwap:
        return QStringLiteral("Possible title/artist swap");
    case MetadataIssueType::MissingArtist:
        return QStringLiteral("Missing artist");
    case MetadataIssueType::MissingTitle:
        return QStringLiteral("Missing title");
    case MetadataIssueType::InvalidMetadata:
        return QStringLiteral("Invalid metadata");
    case MetadataIssueType::LowConfidence:
        return QStringLiteral("Low confidence");
    }
    return QStringLiteral("Unknown");
}

}  // namespace

MetadataRepairReport MetadataRepairReport::fromCandidates(
    const QList<FieldSwapCandidate>& candidates, int totalRecords) {
    MetadataRepairReport report;
    report.totalRecords = totalRecords;
    report.candidates = candidates;

    QHash<MetadataIssueType, MetadataRepairIssueSummary> summaryMap;

    for (const auto& candidate : candidates) {
        auto& summary = summaryMap[candidate.issueType];
        summary.issueType = candidate.issueType;
        summary.count++;
        if (summary.samplePaths.size() < kMaxSamplesPerSummary &&
            !candidate.filePath.isEmpty()) {
            summary.samplePaths << candidate.filePath;
        }

        switch (candidate.issueType) {
        case MetadataIssueType::NoIssue:
            report.noIssueCount++;
            break;
        case MetadataIssueType::PossibleTitleArtistSwap:
            report.possibleSwapCount++;
            if (candidate.isRepairCandidate()) {
                report.repairCandidateCount++;
            }
            if (candidate.shouldSuggestSwap()) {
                report.suggestSwapCount++;
            }
            if (candidate.autoApplyAllowed) {
                report.autoApplyAllowedCount++;
            }
            break;
        case MetadataIssueType::MissingArtist:
            report.missingArtistCount++;
            break;
        case MetadataIssueType::MissingTitle:
            report.missingTitleCount++;
            break;
        case MetadataIssueType::InvalidMetadata:
            report.invalidMetadataCount++;
            break;
        case MetadataIssueType::LowConfidence:
            report.lowConfidenceCount++;
            break;
        }
    }

    report.summaries = summaryMap.values();
    return report;
}

QString MetadataRepairReport::summaryLine() const {
    return QStringLiteral("%1 records, %2 possible swaps, %3 missing artists, %4 repair candidates")
        .arg(totalRecords)
        .arg(possibleSwapCount)
        .arg(missingArtistCount)
        .arg(repairCandidateCount);
}

}  // namespace kivo::metadata