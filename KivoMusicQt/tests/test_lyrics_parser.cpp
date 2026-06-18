// =============================================================================
// Kivo Music Qt - test_lyrics_parser.cpp
// 职责: LyricsParser 单元测试
// =============================================================================

#include <gtest/gtest.h>
#include <QCoreApplication>
#include "../native/audio_core_bridge/lyrics/lyrics_parser.hpp"

using namespace kivo::qt::audio_bridge::lyrics;

// LyricsParser is a static-only utility: QMap<qint64, QString> parse(content).
// No instance is created — every case calls LyricsParser::parse() directly.

TEST(LyricsParserTest, EmptyLrcReturnsEmpty) {
    auto result = LyricsParser::parse("");
    EXPECT_TRUE(result.isEmpty());
}

TEST(LyricsParserTest, ParseSimpleLrc) {
    QString lrc = "[00:01.00]Hello\n[00:05.00]World\n";
    auto result = LyricsParser::parse(lrc);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.value(1000), "Hello");
    EXPECT_EQ(result.value(5000), "World");
}

TEST(LyricsParserTest, ParseMinutesSeconds) {
    QString lrc = "[02:30.50]Middle of song\n";
    auto result = LyricsParser::parse(lrc);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.value(150500), "Middle of song");
}

TEST(LyricsParserTest, SkipsMetadataTags) {
    QString lrc = "[ti:Title]\n[ar:Artist]\n[00:10.00]First line\n";
    auto result = LyricsParser::parse(lrc);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.value(10000), "First line");
}

TEST(LyricsParserTest, HandlesMultipleDigits) {
    QString lrc = "[00:00.00]Start\n[00:00.500]Half second\n";
    auto result = LyricsParser::parse(lrc);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result.value(0), "Start");
    EXPECT_EQ(result.value(500), "Half second");
}

TEST(LyricsParserTest, EmptyLinesIgnored) {
    QString lrc = "[00:01.00]\n[00:02.00]Valid\n";
    auto result = LyricsParser::parse(lrc);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result.value(2000), "Valid");
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
