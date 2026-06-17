#include "swap_detector.hpp"
#include "../validation/metadata_value_validator.hpp"

#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QRegularExpression>

namespace kivo::metadata {

namespace {

bool artistLooksLikeTrackNumber(const QString& artist) {
    static const QRegularExpression trackNumberPattern(
        QStringLiteral("^(\\d{1,3}[\\.\\s]|Track\\s*\\d)"),
        QRegularExpression::CaseInsensitiveOption);
    return trackNumberPattern.match(artist).hasMatch();
}

bool titleLooksLikePlainName(const QString& title) {
    const QString trimmed = title.trimmed();
    if (trimmed.isEmpty()) {
        return false;
    }
    if (trimmed.length() >= 30) {
        return false;
    }
    // Reject only if title starts with a track-number pattern; digits elsewhere
    // (e.g. "U2", "Blink-182") are legitimate artist names.
    static const QRegularExpression trackPrefix(
        QStringLiteral("^(\\d{1,3}[\\.\\s]|Track\\s*\\d)"),
        QRegularExpression::CaseInsensitiveOption);
    if (trackPrefix.match(trimmed).hasMatch()) {
        return false;
    }
    return true;
}

QString albumOrFolderKey(const FieldSwapCandidate& candidate) {
    if (!candidate.originalAlbum.isEmpty()) {
        return candidate.originalAlbum;
    }
    const QFileInfo info(candidate.filePath);
    return info.dir().dirName();
}

}  // namespace

FieldSwapCandidate FieldSwapDetector::detect(const MusicFileRecord& record) {
    FieldSwapCandidate candidate;
    candidate.originalTitle = record.title;
    candidate.originalArtist = record.artist;
    candidate.originalAlbum = record.album;
    candidate.filePath = record.path;
    candidate.suggestedTitle = record.title;
    candidate.suggestedArtist = record.artist;

    if (!MetadataValueValidator::isValidArtist(record.artist)) {
        candidate.issueType = MetadataIssueType::MissingArtist;
        candidate.rejectReasons << QStringLiteral("Artist is empty or a known placeholder");
        return candidate;
    }

    if (!MetadataValueValidator::isValidTitle(record.title)) {
        candidate.issueType = MetadataIssueType::MissingTitle;
        candidate.rejectReasons << QStringLiteral("Title is empty or a known placeholder");
        return candidate;
    }

    if (artistLooksLikeTrackNumber(record.artist) && titleLooksLikePlainName(record.title)) {
        candidate.issueType = MetadataIssueType::PossibleTitleArtistSwap;
        candidate.suggestedTitle = record.artist;
        candidate.suggestedArtist = record.title;
        candidate.confidence = 0.5;
        candidate.evidenceList << QStringLiteral("Artist field begins with a track number pattern");
        candidate.evidenceList << QStringLiteral("Title field resembles a plain artist name");
    } else {
        candidate.issueType = MetadataIssueType::NoIssue;
        candidate.rejectReasons << QStringLiteral("No title/artist swap pattern detected");
    }

    return candidate;
}

QList<FieldSwapCandidate> FieldSwapDetector::detectBatch(const QList<MusicFileRecord>& records) {
    QList<FieldSwapCandidate> candidates;
    candidates.reserve(records.size());

    for (const auto& record : records) {
        candidates.append(detect(record));
    }

    QHash<QString, int> albumTrackCounts;
    QHash<QString, int> albumSwapCounts;

    for (const auto& candidate : candidates) {
        const QString key = albumOrFolderKey(candidate);
        albumTrackCounts[key]++;
        if (candidate.issueType == MetadataIssueType::PossibleTitleArtistSwap) {
            albumSwapCounts[key]++;
        }
    }


    for (auto& candidate : candidates) {
        const QString key = albumOrFolderKey(candidate);
        candidate.albumTrackCount = albumTrackCounts.value(key);
        candidate.swapCandidatesInAlbum = albumSwapCounts.value(key);

        if (candidate.issueType == MetadataIssueType::PossibleTitleArtistSwap) {
            candidate.batchEvidence = candidate.swapCandidatesInAlbum >= 2;
            if (candidate.batchEvidence) {
                candidate.confidence += 0.25;
                candidate.evidenceList << QStringLiteral(
                    "Multiple tracks in the same album/folder show the same swap pattern");
            }
            if (candidate.albumTrackCount >= 2 &&
                candidate.swapCandidatesInAlbum == candidate.albumTrackCount) {
                candidate.confidence += 0.15;
                candidate.evidenceList << QStringLiteral(
                    "All tracks in the album/folder show the swap pattern");
            }
            // Cap below 1.0 so autoApplyAllowed stays false by default.
            candidate.confidence = qBound(0.0, candidate.confidence, 0.95);
        }
    }

    return candidates;
}

}  // namespace kivo::metadata
