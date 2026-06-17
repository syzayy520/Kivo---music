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

TEST(MetadataRepairReviewQueueSorting, OrderingIsStableByDescendingConfidence) {
    const QString album = QStringLiteral("Demo");
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"), album);
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("02. Stuck In A Moment"), album);
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("03. Elevation"), album);
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("04. Solo Track"), album + QStringLiteral("2"));
    records << makeRecord(QStringLiteral("Some Song"), QString(), album);

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(queue.totalItems, 5);
    EXPECT_EQ(queue.possibleSwapCount, 4);
    EXPECT_EQ(queue.missingArtistCount, 1);

    double previousConfidence = 1.0;
    for (const auto& item : queue.items) {
        EXPECT_LE(item.confidence, previousConfidence);
        previousConfidence = item.confidence;
    }
    EXPECT_EQ(queue.items.last().issueType, MetadataIssueType::MissingArtist);
}

TEST(MetadataRepairReviewQueueSorting, EqualConfidencePreservesInputOrder) {
    const QString album = QStringLiteral("Batch");
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. First"), album);
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("02. Second"), album);
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("03. Third"), album);

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(queue.totalItems, 3);
    ASSERT_EQ(queue.items.size(), 3);
    // std::stable_sort keeps equal-confidence items in input order.
    EXPECT_EQ(queue.items[0].originalArtist, QStringLiteral("01. First"));
    EXPECT_EQ(queue.items[1].originalArtist, QStringLiteral("02. Second"));
    EXPECT_EQ(queue.items[2].originalArtist, QStringLiteral("03. Third"));
}

TEST(MetadataRepairReviewQueueSorting, MissingArtistIsNotAccidentallyFilteredOut) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"));
    records << makeRecord(QStringLiteral("Song"), QString());

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(queue.totalItems, 2);
    EXPECT_EQ(queue.possibleSwapCount, 1);
    EXPECT_EQ(queue.missingArtistCount, 1);
    EXPECT_EQ(queue.items.last().issueType, MetadataIssueType::MissingArtist);
}

TEST(MetadataRepairReviewQueueSorting, HigherConfidenceSwapAppearsBeforeLowerConfidence) {
    QList<MusicFileRecord> records;
    // Single-swap confidence 0.5
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Solo Track"));
    // Batch-swap confidence >= 0.75
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("02. Batch One"), QStringLiteral("Batch"));
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("03. Batch Two"), QStringLiteral("Batch"));

    const auto report = reportFor(records);
    const auto queue = MetadataRepairReviewQueue::fromReport(report);

    EXPECT_EQ(queue.totalItems, 3);
    EXPECT_EQ(queue.possibleSwapCount, 3);

    // The batch items (higher confidence) should come before the solo item (lower confidence).
    EXPECT_GT(queue.items[0].confidence, queue.items[2].confidence);
    EXPECT_EQ(queue.items[2].originalArtist, QStringLiteral("01. Solo Track"));
}
