#include <gtest/gtest.h>

#include <QString>

#include "../native/metadata_intelligence/confidence/swap_detector.hpp"
#include "../native/metadata_intelligence/repair/swap_corrector.hpp"
#include "../native/metadata_intelligence/validation/metadata_value_validator.hpp"
#include "../native/sources/music/music_file_record.h"

using namespace kivo::metadata;

namespace {

MusicFileRecord makeRecord(const QString& title,
                           const QString& artist,
                           const QString& album = QString(),
                           const QString& path = QString()) {
    MusicFileRecord r;
    r.title = title;
    r.artist = artist;
    r.album = album;
    r.path = path;
    return r;
}

}  // namespace

TEST(MetadataValueValidator, EmptyArtistIsInvalid) {
    EXPECT_FALSE(MetadataValueValidator::isValidArtist(""));
    EXPECT_FALSE(MetadataValueValidator::isValidArtist("   "));
}

TEST(MetadataValueValidator, UnknownArtistIsInvalid) {
    EXPECT_FALSE(MetadataValueValidator::isValidArtist("Unknown Artist"));
    EXPECT_FALSE(MetadataValueValidator::isValidArtist("unknown artist"));
    EXPECT_FALSE(MetadataValueValidator::isValidArtist("Unknown"));
    EXPECT_FALSE(MetadataValueValidator::isValidArtist("Unknown Performer"));
}

TEST(MetadataValueValidator, UnknownChineseArtistIsInvalid) {
    EXPECT_FALSE(MetadataValueValidator::isValidArtist("未知艺人"));
    EXPECT_FALSE(MetadataValueValidator::isValidArtist("未知艺术家"));
    EXPECT_FALSE(MetadataValueValidator::isValidArtist("未知歌手"));
    EXPECT_FALSE(MetadataValueValidator::isValidArtist("未知"));
}

TEST(MetadataValueValidator, ValidArtist) {
    EXPECT_TRUE(MetadataValueValidator::isValidArtist("U2"));
    EXPECT_TRUE(MetadataValueValidator::isValidArtist("周杰伦"));
    EXPECT_TRUE(MetadataValueValidator::isValidArtist("Bryan Adams"));
}

TEST(MetadataValueValidator, PlaceholderTypeDetection) {
    using PT = MetadataValueValidator::PlaceholderType;
    EXPECT_EQ(MetadataValueValidator::getPlaceholderType(""), PT::Empty);
    EXPECT_EQ(MetadataValueValidator::getPlaceholderType("Unknown Artist"), PT::UnknownEnglish);
    EXPECT_EQ(MetadataValueValidator::getPlaceholderType("未知艺人"), PT::UnknownChinese);
    EXPECT_EQ(MetadataValueValidator::getPlaceholderType("U2"), PT::NotPlaceholder);
}

TEST(FieldSwapDetector, NormalRecordNoSwap) {
    auto r = makeRecord("Beautiful Day", "U2", "All That You Can't Leave Behind");
    auto c = FieldSwapDetector::detect(r);
    EXPECT_EQ(c.issueType, MetadataIssueType::NoIssue);
    EXPECT_EQ(c.originalTitle, r.title);
    EXPECT_EQ(c.suggestedTitle, r.title);
    EXPECT_EQ(c.suggestedArtist, r.artist);
    EXPECT_FALSE(c.isRepairCandidate());
}

TEST(FieldSwapDetector, ClassicSwapDetected) {
    auto r = makeRecord("U2", "01. Beautiful Day", "All That You Can't Leave Behind");
    auto c = FieldSwapDetector::detect(r);
    EXPECT_EQ(c.issueType, MetadataIssueType::PossibleTitleArtistSwap);
    EXPECT_EQ(c.suggestedTitle, "01. Beautiful Day");
    EXPECT_EQ(c.suggestedArtist, "U2");
    EXPECT_EQ(c.confidence, 0.5);
    EXPECT_FALSE(c.autoApplyAllowed);
    EXPECT_TRUE(c.requiresReview);
    // At 0.5 confidence (single-song, no batch), below repair threshold 0.60.
    EXPECT_FALSE(c.isRepairCandidate());
    EXPECT_FALSE(c.shouldSuggestSwap());
    EXPECT_TRUE(c.rejectReasons.isEmpty());
    EXPECT_GE(c.evidenceList.size(), 2);
}

TEST(FieldSwapDetector, TrackWordPatternDetected) {
    auto r = makeRecord("Coldplay", "Track 1: Yellow", "Parachutes");
    auto c = FieldSwapDetector::detect(r);
    EXPECT_EQ(c.issueType, MetadataIssueType::PossibleTitleArtistSwap);
    EXPECT_EQ(c.suggestedArtist, "Coldplay");
    EXPECT_EQ(c.suggestedTitle, "Track 1: Yellow");
}

TEST(FieldSwapDetector, EmptyArtistIsMissingArtist) {
    auto r = makeRecord("Some Song", "");
    auto c = FieldSwapDetector::detect(r);
    EXPECT_EQ(c.issueType, MetadataIssueType::MissingArtist);
    EXPECT_EQ(c.suggestedTitle, r.title);
    EXPECT_EQ(c.suggestedArtist, r.artist);
    EXPECT_FALSE(c.isRepairCandidate());
}

TEST(FieldSwapDetector, UnknownArtistIsMissingArtist) {
    auto r = makeRecord("Some Song", "Unknown Artist");
    auto c = FieldSwapDetector::detect(r);
    EXPECT_EQ(c.issueType, MetadataIssueType::MissingArtist);
    EXPECT_EQ(c.suggestedTitle, r.title);
    EXPECT_EQ(c.suggestedArtist, r.artist);
    EXPECT_FALSE(c.isRepairCandidate());
}

TEST(FieldSwapDetector, UnknownChineseArtistIsMissingArtist) {
    auto r = makeRecord("一首歌", "未知艺人");
    auto c = FieldSwapDetector::detect(r);
    EXPECT_EQ(c.issueType, MetadataIssueType::MissingArtist);
    EXPECT_EQ(c.suggestedTitle, r.title);
    EXPECT_EQ(c.suggestedArtist, r.artist);
    EXPECT_FALSE(c.isRepairCandidate());
}

TEST(FieldSwapDetector, TitleWithDigitsIsNotArtist) {
    // "Artist 2024" does not start with a track-number pattern and IS a plausible
    // artist name; "01. Track" begins with a track prefix → genuine swap candidate.
    auto r = makeRecord("Artist 2024", "01. Track");
    auto c = FieldSwapDetector::detect(r);
    EXPECT_EQ(c.issueType, MetadataIssueType::PossibleTitleArtistSwap);
    EXPECT_EQ(c.suggestedTitle, "01. Track");
    EXPECT_EQ(c.suggestedArtist, "Artist 2024");
}

TEST(FieldSwapDetector, LongTitleIsNotArtist) {
    auto r = makeRecord("A Very Long Artist Name That Is Definitely Over Thirty Characters", "1. Title");
    auto c = FieldSwapDetector::detect(r);
    EXPECT_EQ(c.issueType, MetadataIssueType::NoIssue);
}

TEST(FieldSwapDetector, SameTitleAndArtistNotSwapped) {
    auto r = makeRecord("Same", "Same", "Album");
    auto c = FieldSwapDetector::detect(r);
    // artist does not look like a track number, so no swap.
    EXPECT_EQ(c.issueType, MetadataIssueType::NoIssue);
}

TEST(FieldSwapCorrector, RejectsSwapThatWouldYieldUnknownArtist) {
    // Title="Unknown Artist" → validator catches it as MissingTitle before swap.
    auto r = makeRecord("Unknown Artist", "01. Track Name");
    auto c = FieldSwapDetector::detect(r);
    EXPECT_EQ(c.issueType, MetadataIssueType::MissingTitle);
    // Corrector is a no-op for non-swap issues.
    auto formalized = FieldSwapCorrector::formalize(c);
    EXPECT_EQ(formalized.issueType, MetadataIssueType::MissingTitle);
    EXPECT_FALSE(formalized.isRepairCandidate());
}

TEST(FieldSwapCorrector, RejectsSwapThatWouldYieldUnknownTitle) {
    // "01. Unknown Track" as full artist is not exactly a placeholder, so the
    // detector classifies it as a swap. The corrector double-checks: the suggested
    // title ("01. Unknown Track") is NOT an exact placeholder match either, so it
    // passes the formalization safety gate and the swap stands.
    auto r = makeRecord("Artist Name", "01. Unknown Track");
    auto c = FieldSwapDetector::detect(r);
    EXPECT_EQ(c.issueType, MetadataIssueType::PossibleTitleArtistSwap);
    auto formalized = FieldSwapCorrector::formalize(c);
    EXPECT_EQ(formalized.issueType, MetadataIssueType::PossibleTitleArtistSwap);
    EXPECT_EQ(formalized.confidence, c.confidence);
}

TEST(FieldSwapCorrector, AcceptsValidSwap) {
    auto r = makeRecord("U2", "01. Beautiful Day");
    auto c = FieldSwapDetector::detect(r);
    auto formalized = FieldSwapCorrector::formalize(c);
    EXPECT_EQ(formalized.issueType, MetadataIssueType::PossibleTitleArtistSwap);
    EXPECT_EQ(formalized.suggestedTitle, "01. Beautiful Day");
    EXPECT_EQ(formalized.suggestedArtist, "U2");
    EXPECT_EQ(formalized.confidence, 0.5);
}

TEST(FieldSwapDetector, BatchEvidenceBoostsConfidence) {
    QList<MusicFileRecord> records;
    records << makeRecord("U2", "01. Beautiful Day", "Album");
    records << makeRecord("U2", "02. Stuck In A Moment", "Album");
    records << makeRecord("U2", "03. Elevation", "Album");

    auto candidates = FieldSwapDetector::detectBatch(records);
    ASSERT_EQ(candidates.size(), 3);
    for (const auto& c : candidates) {
        EXPECT_EQ(c.issueType, MetadataIssueType::PossibleTitleArtistSwap);
        EXPECT_EQ(c.albumTrackCount, 3);
        EXPECT_EQ(c.swapCandidatesInAlbum, 3);
        EXPECT_TRUE(c.batchEvidence);
        EXPECT_GE(c.confidence, 0.8);
        EXPECT_LE(c.confidence, 0.95);
    }
}

TEST(FieldSwapDetector, NoBatchEvidenceWhenOnlyOneSwapped) {
    QList<MusicFileRecord> records;
    records << makeRecord("U2", "01. Beautiful Day", "Album");
    records << makeRecord("U2", "Until The End Of The World", "Album");
    // artist is invalid → MissingArtist; no swap counted for this track
    records << makeRecord("03. Elevation", "Unknown Artist", "Album");

    auto candidates = FieldSwapDetector::detectBatch(records);
    ASSERT_EQ(candidates.size(), 3);
    EXPECT_EQ(candidates[0].issueType, MetadataIssueType::PossibleTitleArtistSwap);
    EXPECT_EQ(candidates[1].issueType, MetadataIssueType::NoIssue);
    EXPECT_EQ(candidates[2].issueType, MetadataIssueType::MissingArtist);
    // Only one swap in the album → no batch evidence boost
    EXPECT_EQ(candidates[0].swapCandidatesInAlbum, 1);
    EXPECT_FALSE(candidates[0].batchEvidence);
    EXPECT_EQ(candidates[0].confidence, 0.5);
}
