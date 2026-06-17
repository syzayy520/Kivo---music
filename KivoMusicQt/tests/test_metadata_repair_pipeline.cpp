#include <gtest/gtest.h>

#include "native/metadata_intelligence/pipeline/metadata_repair_pipeline.hpp"
#include "native/metadata_intelligence/report/metadata_repair_report.hpp"
#include "native/metadata_intelligence/validation/metadata_value_validator.hpp"

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

}  // namespace

TEST(MetadataRepairPipeline, EmptyInputReport) {
    const auto report = MetadataRepairPipeline::run(QList<MusicFileRecord>());
    EXPECT_EQ(report.totalRecords, 0);
    EXPECT_EQ(report.noIssueCount, 0);
    EXPECT_EQ(report.possibleSwapCount, 0);
    EXPECT_EQ(report.missingArtistCount, 0);
    EXPECT_TRUE(report.candidates.isEmpty());
    EXPECT_TRUE(report.summaries.isEmpty());
}

TEST(MetadataRepairPipeline, NormalMetadataProducesNoIssue) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("Beautiful Day"), QStringLiteral("U2"));

    const auto report = MetadataRepairPipeline::run(records);
    EXPECT_EQ(report.totalRecords, 1);
    EXPECT_EQ(report.noIssueCount, 1);
    EXPECT_EQ(report.possibleSwapCount, 0);
    EXPECT_EQ(report.missingArtistCount, 0);
    EXPECT_EQ(report.repairCandidateCount, 0);
    ASSERT_EQ(report.candidates.size(), 1);
    EXPECT_EQ(report.candidates[0].issueType, MetadataIssueType::NoIssue);
}

TEST(MetadataRepairPipeline, UnknownArtistIsMissingArtistNotSwap) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("01. Track Name"), QStringLiteral("Unknown Artist"));

    const auto report = MetadataRepairPipeline::run(records);
    EXPECT_EQ(report.totalRecords, 1);
    EXPECT_EQ(report.noIssueCount, 0);
    EXPECT_EQ(report.possibleSwapCount, 0);
    EXPECT_EQ(report.missingArtistCount, 1);
    EXPECT_EQ(report.repairCandidateCount, 0);
    ASSERT_EQ(report.candidates.size(), 1);
    EXPECT_EQ(report.candidates[0].issueType, MetadataIssueType::MissingArtist);
    EXPECT_EQ(report.candidates[0].originalTitle, QStringLiteral("01. Track Name"));
    EXPECT_EQ(report.candidates[0].originalArtist, QStringLiteral("Unknown Artist"));
    // Input must not be modified by the pipeline.
    EXPECT_EQ(records[0].title, QStringLiteral("01. Track Name"));
    EXPECT_EQ(records[0].artist, QStringLiteral("Unknown Artist"));
}

TEST(MetadataRepairPipeline, UnknownChineseArtistIsMissingArtistNotSwap) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("01. 歌曲"), QStringLiteral("未知艺人"));

    const auto report = MetadataRepairPipeline::run(records);
    EXPECT_EQ(report.totalRecords, 1);
    EXPECT_EQ(report.missingArtistCount, 1);
    EXPECT_EQ(report.possibleSwapCount, 0);
    ASSERT_EQ(report.candidates.size(), 1);
    EXPECT_EQ(report.candidates[0].issueType, MetadataIssueType::MissingArtist);
}

TEST(MetadataRepairPipeline, EmptyArtistIsMissingArtistNotSwap) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("Some Song"), QString());

    const auto report = MetadataRepairPipeline::run(records);
    EXPECT_EQ(report.totalRecords, 1);
    EXPECT_EQ(report.missingArtistCount, 1);
    EXPECT_EQ(report.possibleSwapCount, 0);
    ASSERT_EQ(report.candidates.size(), 1);
    EXPECT_EQ(report.candidates[0].issueType, MetadataIssueType::MissingArtist);
}

TEST(MetadataRepairPipeline, PossibleSwapIsReportedAndRequiresReview) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"));

    const auto report = MetadataRepairPipeline::run(records);
    EXPECT_EQ(report.totalRecords, 1);
    EXPECT_EQ(report.noIssueCount, 0);
    EXPECT_EQ(report.possibleSwapCount, 1);
    EXPECT_EQ(report.missingArtistCount, 0);
    ASSERT_EQ(report.candidates.size(), 1);
    const auto& c = report.candidates[0];
    EXPECT_EQ(c.issueType, MetadataIssueType::PossibleTitleArtistSwap);
    EXPECT_EQ(c.suggestedTitle, QStringLiteral("01. Beautiful Day"));
    EXPECT_EQ(c.suggestedArtist, QStringLiteral("U2"));
    EXPECT_EQ(c.confidence, 0.5);
    EXPECT_TRUE(c.requiresReview);
    EXPECT_FALSE(c.autoApplyAllowed);
}

TEST(MetadataRepairPipeline, BatchEvidenceBoostsConfidence) {
    const QString album = QStringLiteral("All That You Can't Leave Behind");
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"), album);
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("02. Stuck In A Moment"), album);
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("03. Elevation"), album);

    const auto report = MetadataRepairPipeline::run(records);
    EXPECT_EQ(report.totalRecords, 3);
    EXPECT_EQ(report.possibleSwapCount, 3);
    EXPECT_EQ(report.noIssueCount, 0);

    for (const auto& c : report.candidates) {
        EXPECT_EQ(c.issueType, MetadataIssueType::PossibleTitleArtistSwap);
        EXPECT_TRUE(c.batchEvidence);
        EXPECT_GE(c.confidence, 0.75);
        EXPECT_LE(c.confidence, 0.95);
        EXPECT_TRUE(c.requiresReview);
        EXPECT_FALSE(c.autoApplyAllowed);
    }
}

TEST(MetadataRepairPipeline, ReportCountsMatchIssueTypes) {
    const QString album = QStringLiteral("Demo");
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("Normal Song"), QStringLiteral("Normal Artist"), album);
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"), album);
    records << makeRecord(QStringLiteral("Song"), QStringLiteral("Unknown Artist"), album);

    const auto report = MetadataRepairPipeline::run(records);
    EXPECT_EQ(report.totalRecords, 3);
    EXPECT_EQ(report.noIssueCount, 1);
    EXPECT_EQ(report.possibleSwapCount, 1);
    EXPECT_EQ(report.missingArtistCount, 1);
    EXPECT_EQ(report.missingArtistCount + report.noIssueCount + report.possibleSwapCount,
              report.totalRecords);

    int sumFromSummaries = 0;
    for (const auto& s : report.summaries) {
        sumFromSummaries += s.count;
    }
    EXPECT_EQ(sumFromSummaries, report.totalRecords);
}

TEST(MetadataRepairPipeline, PipelineDoesNotModifyInputRecords) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"));
    records << makeRecord(QStringLiteral("Song"), QStringLiteral("Unknown Artist"));

    const auto original = records;
    const auto report = MetadataRepairPipeline::run(records);
    EXPECT_EQ(report.totalRecords, 2);

    for (int i = 0; i < records.size(); ++i) {
        EXPECT_EQ(records[i].title, original[i].title);
        EXPECT_EQ(records[i].artist, original[i].artist);
        EXPECT_EQ(records[i].album, original[i].album);
        EXPECT_EQ(records[i].path, original[i].path);
    }
}

TEST(MetadataRepairPipeline, AutoApplyAllowedIsAlwaysFalse) {
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("U2"), QStringLiteral("01. Beautiful Day"));

    const auto report = MetadataRepairPipeline::run(records);
    EXPECT_EQ(report.autoApplyAllowedCount, 0);
    for (const auto& c : report.candidates) {
        EXPECT_FALSE(c.autoApplyAllowed);
    }
}

TEST(MetadataRepairReport, SummaryLineIsHumanReadable) {
    const auto report = MetadataRepairPipeline::run(QList<MusicFileRecord>());
    const auto line = report.summaryLine();
    EXPECT_FALSE(line.isEmpty());
    EXPECT_TRUE(line.contains(QStringLiteral("0 records")));
    EXPECT_TRUE(line.contains(QStringLiteral("0 possible swaps")));
}

TEST(MetadataRepairPipeline, NoDatabaseOrTagWriteOrApplyMusicRecords) {
    // This test is static: it asserts that the pipeline's public surface only
    // depends on detector/corrector/report and the MusicFileRecord value object.
    // It does not (and cannot) link against any repository, ID3 writer, or
    // applyMusicRecords symbol. If the build passes, the contract is upheld.
    QList<MusicFileRecord> records;
    records << makeRecord(QStringLiteral("A"), QStringLiteral("B"));
    const auto report = MetadataRepairPipeline::run(records);
    EXPECT_EQ(report.totalRecords, 1);
}
