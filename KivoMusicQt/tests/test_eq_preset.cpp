// =============================================================================
// Kivo Music Qt - test_eq_preset.cpp
// 职责: EqPresetManager 单元测试
// =============================================================================

#include <gtest/gtest.h>
#include <QCoreApplication>
#include "../native/audio_core_bridge/eq/eq_preset.hpp"

using namespace kivo::qt::audio_bridge::eq;

class EqPresetTest : public ::testing::Test {
protected:
    void SetUp() override { mgr_ = new EqPresetManager(nullptr); }
    void TearDown() override { delete mgr_; }
    EqPresetManager* mgr_;
};

TEST_F(EqPresetTest, HasAllPresets) {
    auto names = mgr_->presetNames();
    EXPECT_TRUE(names.contains("Flat"));
    EXPECT_TRUE(names.contains("Rock"));
    EXPECT_TRUE(names.contains("Pop"));
    EXPECT_TRUE(names.contains("Jazz"));
    EXPECT_TRUE(names.contains("Classical"));
    EXPECT_TRUE(names.contains("Vocal"));
    EXPECT_TRUE(names.contains("Bass Boost"));
    EXPECT_TRUE(names.contains("Treble"));
    EXPECT_EQ(mgr_->presetCount(), 8);
}

TEST_F(EqPresetTest, FlatPresetAllZero) {
    auto gains = mgr_->gainsFor("Flat");
    EXPECT_EQ(gains.size(), 10);
    for (auto g : gains) {
        EXPECT_DOUBLE_EQ(g, 0.0);
    }
}

TEST_F(EqPresetTest, RockPresetHasBassBoost) {
    auto gains = mgr_->gainsFor("Rock");
    EXPECT_GT(gains[0], 0.0);  // 32Hz should be boosted
    EXPECT_GT(gains[1], 0.0);  // 64Hz should be boosted
}

TEST_F(EqPresetTest, BassBoostHasHighLowFreq) {
    auto gains = mgr_->gainsFor("Bass Boost");
    EXPECT_GT(gains[0], 5.0);  // Heavy bass
    EXPECT_DOUBLE_EQ(gains[9], 0.0); // No treble boost
}

TEST_F(EqPresetTest, UnknownPresetReturnsFlat) {
    auto gains = mgr_->gainsFor("Nonexistent");
    for (auto g : gains) {
        EXPECT_DOUBLE_EQ(g, 0.0);
    }
}

TEST_F(EqPresetTest, AllPresetsHave10Bands) {
    for (const auto& name : mgr_->presetNames()) {
        EXPECT_EQ(mgr_->gainsFor(name).size(), 10);
    }
}

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
