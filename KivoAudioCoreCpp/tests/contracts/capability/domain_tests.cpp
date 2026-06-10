// =============================================================================
// KivoAudioCoreCpp — domain_tests.cpp
// Tests for Capability domain contract types
// =============================================================================
//
// Tests: SourceCapability, ClockCapability, ThreadCapability, ConversionCapability
// =============================================================================

#include "kivo/core/contract/capability/domain/source_caps.hpp"
#include "kivo/core/contract/capability/domain/clock.hpp"
#include "kivo/core/contract/capability/domain/thread.hpp"
#include "kivo/core/contract/capability/domain/conversion.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// SourceCapability tests
// =============================================================================
static void source_caps_default_values() {
    SourceCapability s;
    ASSERT(s.source_level == CapabilityLevel::None);
    ASSERT(s.seekable == false);
    ASSERT(s.range_readable == false);
    ASSERT(s.duration_known == false);
}

static void source_caps_uses_capability_level() {
    SourceCapability s;
    s.source_level = CapabilityLevel::High;
    ASSERT(s.source_level == CapabilityLevel::High);
}

static void source_caps_flags() {
    SourceCapability s;
    s.seekable = true;
    s.range_readable = true;
    s.duration_known = true;
    ASSERT(s.seekable);
    ASSERT(s.range_readable);
    ASSERT(s.duration_known);
}

// =============================================================================
// ClockCapability tests
// =============================================================================
static void clock_default_values() {
    ClockCapability c;
    ASSERT(c.clock_level == CapabilityLevel::None);
    ASSERT(c.monotonic == false);
}

static void clock_monotonic_is_invariant_property() {
    // ClockCapability::monotonic is an invariant/truth property,
    // NOT a capability support field.
    ClockCapability c;
    c.clock_level = CapabilityLevel::Standard;
    c.monotonic = true;
    ASSERT(c.monotonic);
    // monotonic is independent of clock_level
    c.clock_level = CapabilityLevel::None;
    ASSERT(c.monotonic);  // Still true — it's a truth property
}

static void clock_uses_capability_level() {
    ClockCapability c;
    c.clock_level = CapabilityLevel::Maximum;
    ASSERT(c.clock_level == CapabilityLevel::Maximum);
}

// =============================================================================
// ThreadCapability tests
// =============================================================================
static void thread_default_values() {
    ThreadCapability t;
    ASSERT(t.thread_level == CapabilityLevel::None);
    ASSERT(t.realtime_safe == false);
}

static void thread_uses_capability_level() {
    // ThreadCapability uses CapabilityLevel, NOT bool supports_*
    ThreadCapability t;
    t.thread_level = CapabilityLevel::High;
    ASSERT(t.thread_level == CapabilityLevel::High);
}

static void thread_realtime_safe() {
    ThreadCapability t;
    t.realtime_safe = true;
    ASSERT(t.realtime_safe);
}

// =============================================================================
// ConversionCapability tests
// =============================================================================
static void conversion_default_values() {
    ConversionCapability c;
    ASSERT(c.conversion_level == CapabilityLevel::None);
    ASSERT(c.resample_capable == false);
    ASSERT(c.channel_remix_capable == false);
}

static void conversion_uses_capability_level() {
    ConversionCapability c;
    c.conversion_level = CapabilityLevel::Standard;
    ASSERT(c.conversion_level == CapabilityLevel::Standard);
}

static void conversion_flags() {
    ConversionCapability c;
    c.resample_capable = true;
    c.channel_remix_capable = true;
    ASSERT(c.resample_capable);
    ASSERT(c.channel_remix_capable);
}

// =============================================================================
// Test runner
// =============================================================================
void run_domain_contract_tests(ContractTestRunner& runner) {
    runner.run("domain::source_caps_default_values", source_caps_default_values);
    runner.run("domain::source_caps_uses_capability_level", source_caps_uses_capability_level);
    runner.run("domain::source_caps_flags", source_caps_flags);
    runner.run("domain::clock_default_values", clock_default_values);
    runner.run("domain::clock_monotonic_is_invariant_property", clock_monotonic_is_invariant_property);
    runner.run("domain::clock_uses_capability_level", clock_uses_capability_level);
    runner.run("domain::thread_default_values", thread_default_values);
    runner.run("domain::thread_uses_capability_level", thread_uses_capability_level);
    runner.run("domain::thread_realtime_safe", thread_realtime_safe);
    runner.run("domain::conversion_default_values", conversion_default_values);
    runner.run("domain::conversion_uses_capability_level", conversion_uses_capability_level);
    runner.run("domain::conversion_flags", conversion_flags);
}
