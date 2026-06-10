// =============================================================================
// KivoAudioCoreCpp — clock_policy_tests.cpp
// Tests for clock policy enum types
// =============================================================================

#include "kivo/core/contract/clock/pause_resume_freeze_policy.hpp"
#include "kivo/core/contract/clock/device_lost_position_report_rule.hpp"
#include "kivo/core/contract/clock/seek_clock_reset_rule.hpp"
#include "kivo/core/contract/clock/drain_eos_timeline_rule.hpp"
#include "kivo/core/contract/clock/gapless_timeline_continuity_rule.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// PauseResumeFreezePolicy Tests
// =============================================================================
static void pause_resume_freeze_policy_values() {
    ASSERT(static_cast<uint8_t>(PauseResumeFreezePolicy::Unknown) == 0);
    ASSERT(PauseResumeFreezePolicy::Unknown != PauseResumeFreezePolicy::FreezeAll);
    ASSERT(PauseResumeFreezePolicy::FreezeAll != PauseResumeFreezePolicy::FreezeDevice);
    ASSERT(PauseResumeFreezePolicy::FreezeDevice != PauseResumeFreezePolicy::FreezeTimeline);
    ASSERT(PauseResumeFreezePolicy::FreezeTimeline != PauseResumeFreezePolicy::None);
}

// =============================================================================
// DeviceLostPositionReportRule Tests
// =============================================================================
static void device_lost_position_report_rule_values() {
    ASSERT(static_cast<uint8_t>(DeviceLostPositionReportRule::Unknown) == 0);
    ASSERT(DeviceLostPositionReportRule::Unknown != DeviceLostPositionReportRule::LastKnown);
    ASSERT(DeviceLostPositionReportRule::LastKnown != DeviceLostPositionReportRule::StreamEstimate);
    ASSERT(DeviceLostPositionReportRule::StreamEstimate != DeviceLostPositionReportRule::Zero);
    ASSERT(DeviceLostPositionReportRule::Zero != DeviceLostPositionReportRule::Invalid);
}

// =============================================================================
// SeekClockResetRule Tests
// =============================================================================
static void seek_clock_reset_rule_values() {
    ASSERT(static_cast<uint8_t>(SeekClockResetRule::Unknown) == 0);
    ASSERT(SeekClockResetRule::Unknown != SeekClockResetRule::ResetAll);
    ASSERT(SeekClockResetRule::ResetAll != SeekClockResetRule::ResetDevice);
    ASSERT(SeekClockResetRule::ResetDevice != SeekClockResetRule::ResetStream);
    ASSERT(SeekClockResetRule::ResetStream != SeekClockResetRule::ResetTimeline);
    ASSERT(SeekClockResetRule::ResetTimeline != SeekClockResetRule::IncrementGeneration);
}

// =============================================================================
// DrainEosTimelineRule Tests
// =============================================================================
static void drain_eos_timeline_rule_values() {
    ASSERT(static_cast<uint8_t>(DrainEosTimelineRule::Unknown) == 0);
    ASSERT(DrainEosTimelineRule::Unknown != DrainEosTimelineRule::HoldAtEnd);
    ASSERT(DrainEosTimelineRule::HoldAtEnd != DrainEosTimelineRule::ResetToStart);
    ASSERT(DrainEosTimelineRule::ResetToStart != DrainEosTimelineRule::ContinueRunning);
}

// =============================================================================
// GaplessTimelineContinuityRule Tests
// =============================================================================
static void gapless_timeline_continuity_rule_values() {
    ASSERT(static_cast<uint8_t>(GaplessTimelineContinuityRule::Unknown) == 0);
    ASSERT(GaplessTimelineContinuityRule::Unknown != GaplessTimelineContinuityRule::Continuous);
    ASSERT(GaplessTimelineContinuityRule::Continuous != GaplessTimelineContinuityRule::ResetPerTrack);
    ASSERT(GaplessTimelineContinuityRule::ResetPerTrack != GaplessTimelineContinuityRule::OffsetPerTrack);
}

// =============================================================================
// Runner
// =============================================================================
void run_clock_policy_tests(ContractTestRunner& runner) {
    std::cout << "--- PauseResumeFreezePolicy ---\n";
    runner.run("pause_resume_freeze_policy_values", pause_resume_freeze_policy_values);
    std::cout << "\n";

    std::cout << "--- DeviceLostPositionReportRule ---\n";
    runner.run("device_lost_position_report_rule_values", device_lost_position_report_rule_values);
    std::cout << "\n";

    std::cout << "--- SeekClockResetRule ---\n";
    runner.run("seek_clock_reset_rule_values", seek_clock_reset_rule_values);
    std::cout << "\n";

    std::cout << "--- DrainEosTimelineRule ---\n";
    runner.run("drain_eos_timeline_rule_values", drain_eos_timeline_rule_values);
    std::cout << "\n";

    std::cout << "--- GaplessTimelineContinuityRule ---\n";
    runner.run("gapless_timeline_continuity_rule_values", gapless_timeline_continuity_rule_values);
    std::cout << "\n";
}
