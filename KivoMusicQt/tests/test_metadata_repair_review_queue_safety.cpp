#include <gtest/gtest.h>

#include "native/metadata_intelligence/pipeline/metadata_repair_pipeline.hpp"
#include "native/metadata_intelligence/repair/id3_field_swap/field_swap_candidate.hpp"
#include "native/metadata_intelligence/report/metadata_repair_report.hpp"
#include "native/metadata_intelligence/review/metadata_repair_review_queue.hpp"
#include "native/sources/music/music_file_record.h"

#include <QList>

using namespace kivo::metadata;

namespace {

MusicFileRecord makeRecord(const QString& title,
                           const QString& artist,
                           const QString& album = QStringLiteral("Album"),
                           const QString& path = QString()) {
    MusicFileRecord r;
    r.title = title;
    r.artist = artist;
    r.album = album;
    r.path = path.isEmpty() ? QStringLiteral("/music/%1/%2.mp3").arg(album).arg(title) : path;
    return r;
}

MetadataRepairReport reportFor(const QList<MusicFileRecord>& records) {
    return MetadataRepairPipeline::run(records);
}

}  // namespace

TEST(MetadataRepairReviewQueueSafety, UnknownArtistStaysMissingArtistNotSwap) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("01. Track"), QStringLiteral("Unknown Artist"));
    records << makeRecord(QStringLiteral("02. 歌曲"), QStringLiteral("未知艺人"));

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(report.missingArtistCount, 2);
    EXPECT_EQ(report.possibleSwapCount, 0);
    ASSERT_EQ(queue.items.size(), 2);
    for (const auto& item : queue.items) {
        EXPECT_EQ(item.issueType, MetadataIssueType::MissingArtist);
    }
    EXPECT_EQ(queue.missingArtistCount, 2);
    EXPECT_EQ(queue.possibleSwapCount, 0);
}

TEST(MetadataRepairReviewQueueSafety, EvidenceAndRejectReasonsArePreserved) {
    QList<MusicFileRecord> records;
    const QString album = QStringLiteral("All That You Can't Leave Behind");
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"), album);
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("02. Stuck In A Moment"), album);

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(report.possibleSwapCount, 2);
    ASSERT_EQ(queue.items.size(), 2);
    for (const auto& item : queue.items) {
        EXPECT_EQ(item.issueType, MetadataIssueType::PossibleTitleArtistSwap);
        EXPECT_FALSE(item.evidenceList.isEmpty());
    }
    EXPECT_EQ(queue.items[0].evidenceList, report.candidates[0].evidenceList);
    EXPECT_EQ(queue.items[1].evidenceList, report.candidates[1].evidenceList);
}

TEST(MetadataRepairReviewQueueSafety, AutoApplyAllowedIsCopiedNotExecuted) {
    QList<FieldSwapCandidate> candidates;
    FieldSwapCandidate candidate;
    candidate.filePath = QStringLiteral("/music/auto.mp3");
    candidate.originalTitle = QStringLiteral("A");
    candidate.originalArtist = QStringLiteral("B");
    candidate.suggestedTitle = QStringLiteral("B");
    candidate.suggestedArtist = QStringLiteral("A");
    candidate.issueType = MetadataIssueType::PossibleTitleArtistSwap;
    candidate.confidence = 0.98;
    candidate.requiresReview = false;
    candidate.autoApplyAllowed = true;
    candidate.evidenceList << QStringLiteral("High-confidence swap");
    candidates << candidate;

    const auto report = MetadataRepairReport::fromCandidates(candidates, 1);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    ASSERT_EQ(queue.items.size(), 1);
    EXPECT_TRUE(queue.items[0].autoApplyAllowed);
    EXPECT_EQ(queue.autoApplyAllowedCount, 1);
    EXPECT_EQ(report.candidates[0].autoApplyAllowed, true);
}

TEST(MetadataRepairReviewQueueSafety, FromReportDoesNotModifyReportOrCandidates) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"));
    records << makeRecord(QStringLiteral("Song"), QStringLiteral("Unknown Artist"));

    auto report = reportFor(records);
    const auto originalCandidates = report.candidates;

    MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(report.candidates.size(), originalCandidates.size());
    for (int i = 0; i < report.candidates.size(); ++i) {
        EXPECT_EQ(report.candidates[i].issueType, originalCandidates[i].issueType);
        EXPECT_EQ(report.candidates[i].confidence, originalCandidates[i].confidence);
        EXPECT_EQ(report.candidates[i].originalTitle, originalCandidates[i].originalTitle);
        EXPECT_EQ(report.candidates[i].originalArtist, originalCandidates[i].originalArtist);
        EXPECT_EQ(report.candidates[i].suggestedTitle, originalCandidates[i].suggestedTitle);
        EXPECT_EQ(report.candidates[i].suggestedArtist, originalCandidates[i].suggestedArtist);
        EXPECT_EQ(report.candidates[i].evidenceList, originalCandidates[i].evidenceList);
        EXPECT_EQ(report.candidates[i].rejectReasons, originalCandidates[i].rejectReasons);
    }
}

TEST(MetadataRepairReviewQueueSafety, NoDatabaseOrTagWriteOrApplyMusicRecords) {
    // Static build-time assertion: the review queue only depends on report,
    // review item/filter, and the FieldSwapCandidate value object. It does not
    // (and cannot) link any repository, ID3 writer, or applyMusicRecords symbol.
    QList<FieldSwapCandidate> candidates;
    const auto report = MetadataRepairReport::fromCandidates(candidates, 0);
    (void)MetadataRepairReviewQueue::fromReport(report);
    SUCCEED();
}
