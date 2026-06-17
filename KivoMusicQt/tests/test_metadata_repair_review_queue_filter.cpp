#include <gtest/gtest.h>

#include "native/metadata_intelligence/pipeline/metadata_repair_pipeline.hpp"
#include "native/metadata_intelligence/report/metadata_repair_report.hpp"
#include "native/metadata_intelligence/review/metadata_repair_review_filter.hpp"
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

TEST(MetadataRepairReviewQueueFilter, IncludeNoIssueFilterAllowsNoIssueItems) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("Beautiful Day"), QStringLiteral("U2"));

    const auto report = reportFor(records);
    MetadataRepairReviewFilter filter;
    filter.includeNoIssue = true;
    const auto queue = MetadataRepairReviewQueue::fromReport(report, filter);

    ASSERT_EQ(queue.items.size(), 1);
    EXPECT_EQ(queue.items[0].issueType, MetadataIssueType::NoIssue);
    EXPECT_EQ(queue.noIssueCount, 1);
    EXPECT_FALSE(queue.items[0].autoApplyAllowed);
}

TEST(MetadataRepairReviewQueueFilter, ExcludeMissingArtistFilterRemovesMissingArtist) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("Song Title"), QString());

    const auto report = reportFor(records);
    MetadataRepairReviewFilter filter;
    filter.includeMissingArtist = false;
    const auto queue = MetadataRepairReviewQueue::fromReport(report, filter);

    EXPECT_TRUE(queue.items.isEmpty());
    EXPECT_EQ(queue.missingArtistCount, 0);
}

TEST(MetadataRepairReviewQueueFilter, ExcludePossibleSwapFilterRemovesPossibleSwap) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"));

    const auto report = reportFor(records);
    MetadataRepairReviewFilter filter;
    filter.includePossibleSwap = false;
    const auto queue = MetadataRepairReviewQueue::fromReport(report, filter);

    EXPECT_TRUE(queue.items.isEmpty());
    EXPECT_EQ(queue.possibleSwapCount, 0);
}

TEST(MetadataRepairReviewQueueFilter, MinimumConfidenceFilterTakesEffect) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"));

    const auto report = reportFor(records);
    EXPECT_EQ(report.possibleSwapCount, 1);
    EXPECT_EQ(report.candidates[0].confidence, 0.5);

    MetadataRepairReviewFilter filter;
    filter.includePossibleSwap = true;
    filter.minimumConfidence = 0.6;
    const auto queue = MetadataRepairReviewQueue::fromReport(report, filter);

    EXPECT_TRUE(queue.items.isEmpty());
    EXPECT_EQ(queue.possibleSwapCount, 0);
}

TEST(MetadataRepairReviewQueueFilter, FilterDoesNotModifyReportOrCandidates) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"));
    records << makeRecord(QStringLiteral("Song"), QStringLiteral("Unknown Artist"));

    auto report = reportFor(records);
    const auto originalCandidates = report.candidates;
    const auto originalNoIssueCount = report.noIssueCount;
    const auto originalPossibleSwapCount = report.possibleSwapCount;
    const auto originalMissingArtistCount = report.missingArtistCount;

    MetadataRepairReviewFilter filter;
    filter.includePossibleSwap = true;
    filter.includeMissingArtist = true;
    MetadataRepairReviewQueue::fromReport(report, filter);

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
    EXPECT_EQ(report.noIssueCount, originalNoIssueCount);
    EXPECT_EQ(report.possibleSwapCount, originalPossibleSwapCount);
    EXPECT_EQ(report.missingArtistCount, originalMissingArtistCount);
}
