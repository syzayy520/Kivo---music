#pragma once

#include <QString>
#include <QStringList>

namespace kivo::metadata {

// Issue type classification
enum class MetadataIssueType {
    NoIssue,                    // No problem detected
    PossibleTitleArtistSwap,    // title/artist fields may be swapped
    MissingArtist,              // artist is empty/Unknown/未知艺人
    MissingTitle,               // title is empty/Unknown
    InvalidMetadata,            // Corrupted or invalid data
    LowConfidence               // Issue detected but confidence too low
};

// Field swap repair candidate with evidence and confidence
struct FieldSwapCandidate {
    // Original values from file
    QString originalTitle;
    QString originalArtist;
    QString originalAlbum;
    QString filePath;

    // Suggested corrected values (may be same as original if no swap needed)
    QString suggestedTitle;
    QString suggestedArtist;

    // Issue classification
    MetadataIssueType issueType = MetadataIssueType::NoIssue;

    // Confidence score (0.0 to 1.0)
    // - >= 0.95 with batch evidence: may allow auto-apply (future phase)
    // - 0.80 to 0.95: requires manual review
    // - < 0.80: report only, no repair suggestion
    double confidence = 0.0;

    // Evidence supporting the swap hypothesis
    QStringList evidenceList;

    // Reasons why swap was rejected (if applicable)
    QStringList rejectReasons;

    // Safety flags
    bool requiresReview = true;      // Must be reviewed before applying
    bool autoApplyAllowed = false;   // Safe for automatic application (future)
    bool batchEvidence = false;      // Supported by multiple files in same context

    // Context for batch detection
    int albumTrackCount = 1;         // How many tracks share this album/folder
    int swapCandidatesInAlbum = 0;   // How many tracks in album show same swap pattern

    // Helper: Is this a valid repair candidate?
    bool isRepairCandidate() const {
        return issueType == MetadataIssueType::PossibleTitleArtistSwap
               && confidence >= 0.60;  // Minimum confidence for reporting
    }

    // Helper: Should suggest swap to user?
    bool shouldSuggestSwap() const {
        return isRepairCandidate() && confidence >= 0.80;
    }
};

}  // namespace kivo::metadata
