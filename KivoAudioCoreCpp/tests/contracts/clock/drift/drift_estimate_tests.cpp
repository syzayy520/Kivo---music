// =============================================================================
// KivoAudioCoreCpp — drift_estimate_tests.cpp
// Tests for DriftEstimate contract type
// =============================================================================

#include "kivo/core/contract/clock/drift_estimate.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// DriftDirection Tests
// =============================================================================
static void drift_direction_values() {
    ASSERT(static_cast<uint8_t>(DriftDirection::Unknown) == 0);
    ASSERT(DriftDirection::Unknown != DriftDirection::None);
    ASSERT(DriftDirection::None != DriftDirection::DeviceAhead);
    ASSERT(DriftDirection::DeviceAhead != DriftDirection::StreamAhead);
}

// =============================================================================
// DriftEstimate Tests
// =============================================================================
static void default_construction_is_invalid() {
    DriftEstimate drift;
    ASSERT(!drift.is_valid());
    ASSERT(drift.direction == DriftDirection::Unknown);
    ASSERT(drift.drift_samples == 0);
    ASSERT(drift.confidence_pct == 0);
}

static void valid_construction_none() {
    DriftEstimate drift;
    drift.direction = DriftDirection::None;
    drift.drift_samples = 0;
    drift.confidence_pct = 100;
    ASSERT(drift.is_valid());
}

static void valid_construction_device_ahead() {
    DriftEstimate drift;
    drift.direction = DriftDirection::DeviceAhead;
    drift.drift_samples = 100;
    drift.confidence_pct = 90;
    ASSERT(drift.is_valid());
}

static void valid_construction_stream_ahead() {
    DriftEstimate drift;
    drift.direction = DriftDirection::StreamAhead;
    drift.drift_samples = -50;
    drift.confidence_pct = 80;
    ASSERT(drift.is_valid());
}

static void invalid_unknown_direction() {
    DriftEstimate drift;
    drift.direction = DriftDirection::Unknown;
    drift.drift_samples = 100;
    drift.confidence_pct = 100;
    ASSERT(!drift.is_valid());
}

static void invalid_confidence_over_100() {
    DriftEstimate drift;
    drift.direction = DriftDirection::None;
    drift.drift_samples = 0;
    drift.confidence_pct = 101;
    ASSERT(!drift.is_valid());
}

static void significance_device_ahead() {
    DriftEstimate drift;
    drift.direction = DriftDirection::DeviceAhead;
    drift.drift_samples = 100;
    drift.confidence_pct = 90;

    ASSERT(drift.is_significant(50));
    ASSERT(!drift.is_significant(200));
}

static void significance_stream_ahead() {
    DriftEstimate drift;
    drift.direction = DriftDirection::StreamAhead;
    drift.drift_samples = -100;
    drift.confidence_pct = 90;

    ASSERT(drift.is_significant(50));
    ASSERT(!drift.is_significant(200));
}

static void significance_none() {
    DriftEstimate drift;
    drift.direction = DriftDirection::None;
    drift.drift_samples = 0;
    drift.confidence_pct = 100;

    ASSERT(!drift.is_significant(0));
}

static void significance_unknown() {
    DriftEstimate drift;
    drift.direction = DriftDirection::Unknown;
    drift.drift_samples = 100;
    drift.confidence_pct = 90;

    ASSERT(!drift.is_significant(50));
}

static void equality_same() {
    DriftEstimate a;
    a.direction = DriftDirection::DeviceAhead;
    a.drift_samples = 100;
    a.confidence_pct = 90;

    DriftEstimate b = a;
    ASSERT(a == b);
}

static void equality_different() {
    DriftEstimate a;
    a.direction = DriftDirection::DeviceAhead;
    a.drift_samples = 100;
    a.confidence_pct = 90;

    DriftEstimate b;
    b.direction = DriftDirection::StreamAhead;
    b.drift_samples = -100;
    b.confidence_pct = 90;

    ASSERT(!(a == b));
}

// =============================================================================
// Runner
// =============================================================================
void run_drift_estimate_tests(ContractTestRunner& runner) {
    std::cout << "--- DriftDirection ---\n";
    runner.run("drift_direction_values", drift_direction_values);
    std::cout << "\n";

    std::cout << "--- DriftEstimate ---\n";
    runner.run("default_construction_is_invalid", default_construction_is_invalid);
    runner.run("valid_construction_none", valid_construction_none);
    runner.run("valid_construction_device_ahead", valid_construction_device_ahead);
    runner.run("valid_construction_stream_ahead", valid_construction_stream_ahead);
    runner.run("invalid_unknown_direction", invalid_unknown_direction);
    runner.run("invalid_confidence_over_100", invalid_confidence_over_100);
    runner.run("significance_device_ahead", significance_device_ahead);
    runner.run("significance_stream_ahead", significance_stream_ahead);
    runner.run("significance_none", significance_none);
    runner.run("significance_unknown", significance_unknown);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
