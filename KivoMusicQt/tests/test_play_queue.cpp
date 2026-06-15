// =============================================================================
// Kivo Music Qt - test_play_queue.cpp
// 职责: PlayQueue 单元测试
// =============================================================================

#include <gtest/gtest.h>
#include <QCoreApplication>
#include "../native/audio_core_bridge/queue/play_queue.hpp"

using namespace kivo::qt::audio_bridge;

class PlayQueueTest : public ::testing::Test {
protected:
    void SetUp() override {
        queue_ = new PlayQueue(nullptr);
    }
    void TearDown() override {
        delete queue_;
    }
    PlayQueue* queue_;
};

TEST_F(PlayQueueTest, EmptyOnCreation) {
    EXPECT_EQ(queue_->count(), 0);
    EXPECT_TRUE(queue_->isEmpty());
    EXPECT_EQ(queue_->currentIndex(), -1);
    EXPECT_FALSE(queue_->hasNext());
    EXPECT_FALSE(queue_->hasPrevious());
}

TEST_F(PlayQueueTest, AddSingleTrack) {
    queue_->addTrack("/music/song.flac");
    EXPECT_EQ(queue_->count(), 1);
    EXPECT_FALSE(queue_->isEmpty());
    EXPECT_EQ(queue_->currentIndex(), 0);
    EXPECT_EQ(queue_->currentTrack(), "/music/song.flac");
}

TEST_F(PlayQueueTest, AddMultipleTracks) {
    QStringList tracks = {"/a.flac", "/b.mp3", "/c.wav"};
    queue_->addTracks(tracks);
    EXPECT_EQ(queue_->count(), 3);
    EXPECT_EQ(queue_->currentIndex(), 0);
}

TEST_F(PlayQueueTest, NextAndPrevious) {
    queue_->addTracks({"/a.flac", "/b.flac", "/c.flac"});
    EXPECT_TRUE(queue_->next());
    EXPECT_EQ(queue_->currentIndex(), 1);
    EXPECT_TRUE(queue_->next());
    EXPECT_EQ(queue_->currentIndex(), 2);
    EXPECT_FALSE(queue_->next());
    EXPECT_TRUE(queue_->previous());
    EXPECT_EQ(queue_->currentIndex(), 1);
    EXPECT_TRUE(queue_->previous());
    EXPECT_EQ(queue_->currentIndex(), 0);
    EXPECT_FALSE(queue_->previous());
}

TEST_F(PlayQueueTest, RemoveTrack) {
    queue_->addTracks({"/a.flac", "/b.flac", "/c.flac"});
    queue_->removeTrack(1);
    EXPECT_EQ(queue_->count(), 2);
    EXPECT_EQ(queue_->getTrackAt(0), "/a.flac");
    EXPECT_EQ(queue_->getTrackAt(1), "/c.flac");
}

TEST_F(PlayQueueTest, Clear) {
    queue_->addTracks({"/a.flac", "/b.flac"});
    queue_->clear();
    EXPECT_TRUE(queue_->isEmpty());
    EXPECT_EQ(queue_->count(), 0);
    EXPECT_EQ(queue_->currentIndex(), -1);
}

TEST_F(PlayQueueTest, ModelRoles) {
    queue_->addTrack("/music/test.mp3");
    QModelIndex idx = queue_->index(0);
    EXPECT_EQ(queue_->data(idx, Qt::UserRole + 1).toString(), "/music/test.mp3");
    EXPECT_FALSE(queue_->data(idx, Qt::UserRole + 2).toString().isEmpty());
}

TEST_F(PlayQueueTest, MoveTrack) {
    queue_->addTracks({"/a.flac", "/b.flac", "/c.flac"});
    queue_->moveTrack(0, 2);
    EXPECT_EQ(queue_->getTrackAt(0), "/b.flac");
    EXPECT_EQ(queue_->getTrackAt(1), "/c.flac");
    EXPECT_EQ(queue_->getTrackAt(2), "/a.flac");
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
