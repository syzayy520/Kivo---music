// =============================================================================
// Kivo Music Qt - test_playback_mode.cpp
// 职责: PlaybackMode 单元测试
// =============================================================================

#include <gtest/gtest.h>
#include <QCoreApplication>
#include "../native/audio_core_bridge/queue/playback_mode.hpp"

using namespace kivo::qt::audio_bridge;

class PlaybackModeTest : public ::testing::Test {
protected:
    void SetUp() override { mode_ = new PlaybackMode(nullptr); }
    void TearDown() override { delete mode_; }
    PlaybackMode* mode_;
};

TEST_F(PlaybackModeTest, DefaultIsSequential) {
    EXPECT_EQ(mode_->mode(), PlaybackMode::Mode::Sequential);
}

TEST_F(PlaybackModeTest, ToggleCyclesModes) {
    EXPECT_EQ(mode_->mode(), PlaybackMode::Mode::Sequential);
    mode_->toggleMode();
    EXPECT_EQ(mode_->mode(), PlaybackMode::Mode::Shuffle);
    mode_->toggleMode();
    EXPECT_EQ(mode_->mode(), PlaybackMode::Mode::RepeatOne);
    mode_->toggleMode();
    EXPECT_EQ(mode_->mode(), PlaybackMode::Mode::RepeatAll);
    mode_->toggleMode();
    EXPECT_EQ(mode_->mode(), PlaybackMode::Mode::Sequential);
}

TEST_F(PlaybackModeTest, SetModeDirectly) {
    mode_->setMode(PlaybackMode::Mode::RepeatOne);
    EXPECT_EQ(mode_->mode(), PlaybackMode::Mode::RepeatOne);
    EXPECT_EQ(mode_->modeString(), "Repeat One");
}

TEST_F(PlaybackModeTest, SequentialNextIndex) {
    EXPECT_EQ(mode_->nextIndex(0, 5), 1);
    EXPECT_EQ(mode_->nextIndex(4, 5), -1); // End of list
}

TEST_F(PlaybackModeTest, SequentialPreviousIndex) {
    EXPECT_EQ(mode_->previousIndex(2, 5), 1);
    EXPECT_EQ(mode_->previousIndex(0, 5), -1);
}

TEST_F(PlaybackModeTest, RepeatAllWraps) {
    mode_->setMode(PlaybackMode::Mode::RepeatAll);
    EXPECT_EQ(mode_->nextIndex(4, 5), 0);
    EXPECT_EQ(mode_->previousIndex(0, 5), 4);
}

TEST_F(PlaybackModeTest, ShuffleInRange) {
    mode_->setMode(PlaybackMode::Mode::Shuffle);
    for (int i = 0; i < 100; i++) {
        int idx = mode_->nextIndex(0, 10);
        EXPECT_GE(idx, 0);
        EXPECT_LT(idx, 10);
    }
}

// Regression guard for the end-of-stream auto-advance bug: onEndOfStream() used
// to call PlayQueue::next() directly, so Shuffle silently degraded to "play the
// next sequential track" (always currentIndex+1). The fix routes auto-advance
// through nextIndex(), which for Shuffle must scatter across the queue and never
// lock onto currentIndex+1. P(false failure) = (1/10)^100 ≈ 0, so this is not
// flaky despite asserting on randomness.
TEST_F(PlaybackModeTest, ShuffleDoesNotDeterministicallyPickNextSequential) {
    mode_->setMode(PlaybackMode::Mode::Shuffle);
    constexpr int kCurrentIndex = 3;
    constexpr int kTotalCount = 10;
    bool sawNonSequential = false;
    for (int i = 0; i < 100; ++i) {
        const int idx = mode_->nextIndex(kCurrentIndex, kTotalCount);
        ASSERT_GE(idx, 0);
        ASSERT_LT(idx, kTotalCount);
        if (idx != kCurrentIndex + 1) {
            sawNonSequential = true;
        }
    }
    EXPECT_TRUE(sawNonSequential)
        << "Shuffle nextIndex() returned currentIndex+1 on every one of 100 "
           "draws — auto-advance is behaving sequentially, not shuffling.";
}

TEST_F(PlaybackModeTest, ModeStrings) {
    mode_->setMode(PlaybackMode::Mode::Sequential);
    EXPECT_EQ(mode_->modeString(), "Sequential");
    mode_->setMode(PlaybackMode::Mode::Shuffle);
    EXPECT_EQ(mode_->modeString(), "Shuffle");
    mode_->setMode(PlaybackMode::Mode::RepeatOne);
    EXPECT_EQ(mode_->modeString(), "Repeat One");
    mode_->setMode(PlaybackMode::Mode::RepeatAll);
    EXPECT_EQ(mode_->modeString(), "Repeat All");
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
