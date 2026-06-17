#include <gtest/gtest.h>

#include "native/metadata_intelligence/pipeline/metadata_repair_pipeline.hpp"
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

TEST(MetadataRepairReviewQueueBasic, EmptyReportProducesEmptyQueue) {
    const auto report = reportFor(QList<MusicFileRecord>());
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_TRUE(queue.items.isEmpty());
    EXPECT_EQ(queue.totalItems, 0);
    EXPECT_EQ(queue.noIssueCount, 0);
    EXPECT_EQ(queue.missingArtistCount, 0);
    EXPECT_EQ(queue.missingTitleCount, 0);
    EXPECT_EQ(queue.possibleSwapCount, 0);
    EXPECT_EQ(queue.invalidMetadataCount, 0);
    EXPECT_EQ(queue.requiresReviewCount, 0);
    EXPECT_EQ(queue.autoApplyAllowedCount, 0);
}

TEST(MetadataRepairReviewQueueBasic, NoIssueIsExcludedByDefault) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("Beautiful Day"), QStringLiteral("U2"));

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(report.noIssueCount, 1);
    EXPECT_TRUE(queue.items.isEmpty());
    EXPECT_EQ(queue.totalItems, 0);
    EXPECT_EQ(queue.noIssueCount, 0);
}

TEST(MetadataRepairReviewQueueBasic, MissingArtistEntersQueue) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("Song Title"), QString());

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(report.missingArtistCount, 1);
    ASSERT_EQ(queue.items.size(), 1);
    EXPECT_EQ(queue.items[0].issueType, MetadataIssueType::MissingArtist);
    EXPECT_EQ(queue.items[0].originalTitle, QStringLiteral("Song Title"));
    EXPECT_TRUE(queue.items[0].originalArtist.isEmpty());
    EXPECT_EQ(queue.items[0].suggestedTitle, queue.items[0].originalTitle);
    EXPECT_EQ(queue.items[0].suggestedArtist, queue.items[0].originalArtist);
    EXPECT_TRUE(queue.items[0].requiresReview);
    EXPECT_FALSE(queue.items[0].autoApplyAllowed);
    EXPECT_EQ(queue.missingArtistCount, 1);
}

TEST(MetadataRepairReviewQueueBasic, MissingTitleEntersQueue) {
    QList<MusicFileRecord> records;
    records << makeRecord(QString(), QStringLiteral("Artist"));

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(report.missingTitleCount, 1);
    ASSERT_EQ(queue.items.size(), 1);
    EXPECT_EQ(queue.items[0].issueType, MetadataIssueType::MissingTitle);
    EXPECT_TRUE(queue.items[0].originalTitle.isEmpty());
    EXPECT_EQ(queue.items[0].originalArtist, QStringLiteral("Artist"));
    EXPECT_EQ(queue.missingTitleCount, 1);
}

TEST(MetadataRepairReviewQueueBasic, PossibleSwapRetainsOriginalAndSuggestedFields) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"));

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(report.possibleSwapCount, 1);
    ASSERT_EQ(queue.items.size(), 1);
    const auto& item = queue.items[0];
    EXPECT_EQ(item.issueType, MetadataIssueType::PossibleTitleArtistSwap);
    EXPECT_EQ(item.originalTitle, QStringLiteral("U2"));
    EXPECT_EQ(item.originalArtist, QStringLiteral("01. Beautiful Day"));
    EXPECT_EQ(item.suggestedTitle, QStringLiteral("01. Beautiful Day"));
    EXPECT_EQ(item.suggestedArtist, QStringLiteral("U2"));
    EXPECT_TRUE(item.requiresReview);
    EXPECT_FALSE(item.autoApplyAllowed);
    EXPECT_FALSE(item.displayReason.isEmpty());
}

TEST(MetadataRepairReviewQueueBasic, DisplayReasonIsHumanReadable) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("Song Title"), QString());

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    ASSERT_EQ(queue.items.size(), 1);
    EXPECT_EQ(queue.items[0].displayReason, QStringLiteral("Missing or placeholder artist"));
}
