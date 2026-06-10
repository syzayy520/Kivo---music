// =============================================================================
// KivoAudioCoreCpp — device_lost_position_report_rule_tests.cpp
// Tests for DeviceLostPositionReportRule enum
// =============================================================================

#include "kivo/core/contract/clock/policy/device_lost_position_report_rule.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void device_lost_position_report_rule_values() {
    ASSERT(static_cast<uint8_t>(DeviceLostPositionReportRule::Unknown) == 0);
    ASSERT(DeviceLostPositionReportRule::Unknown != DeviceLostPositionReportRule::LastKnown);
    ASSERT(DeviceLostPositionReportRule::LastKnown != DeviceLostPositionReportRule::StreamEstimate);
    ASSERT(DeviceLostPositionReportRule::StreamEstimate != DeviceLostPositionReportRule::Zero);
    ASSERT(DeviceLostPositionReportRule::Zero != DeviceLostPositionReportRule::Invalid);
}

// =============================================================================
// Runner
// =============================================================================
void run_device_lost_position_report_rule_tests(ContractTestRunner& runner) {
    std::cout << "--- DeviceLostPositionReportRule ---\n";
    runner.run("device_lost_position_report_rule_values", device_lost_position_report_rule_values);
    std::cout << "\n";
}
