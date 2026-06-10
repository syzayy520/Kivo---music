// =============================================================================
// KivoAudioCoreCpp — constraint_tests.cpp
// Tests for Capability constraint contract types
// =============================================================================
//
// Tests: ConstraintStrictness, MemoryCapability, BufferCapability
// =============================================================================

#include "kivo/core/contract/capability/constraint/strictness.hpp"
#include "kivo/core/contract/capability/constraint/memory.hpp"
#include "kivo/core/contract/capability/constraint/buffer.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// ConstraintStrictness tests
// =============================================================================
static void strictness_has_all_values() {
    ASSERT(ConstraintStrictness::Advisory != ConstraintStrictness::Preferred);
    ASSERT(ConstraintStrictness::Preferred != ConstraintStrictness::Required);
    ASSERT(ConstraintStrictness::Required != ConstraintStrictness::Absolute);
}

static void strictness_equality() {
    ASSERT(ConstraintStrictness::Advisory == ConstraintStrictness::Advisory);
    ASSERT(ConstraintStrictness::Advisory != ConstraintStrictness::Absolute);
}

// =============================================================================
// MemoryCapability tests
// =============================================================================
static void memory_default_values() {
    MemoryCapability m;
    ASSERT(m.memory_level == CapabilityLevel::None);
    ASSERT(m.pool_budget_hint_bytes == 0);
}

static void memory_uses_capability_level() {
    // Verify MemoryCapability uses CapabilityLevel, not bool
    MemoryCapability m;
    m.memory_level = CapabilityLevel::High;
    ASSERT(m.memory_level == CapabilityLevel::High);
    ASSERT(m.memory_level != CapabilityLevel::None);
}

static void memory_equality() {
    MemoryCapability a{CapabilityLevel::Standard, 1024};
    MemoryCapability b{CapabilityLevel::Standard, 1024};
    MemoryCapability c{CapabilityLevel::High, 2048};
    ASSERT(a == b);
    ASSERT(a != c);
}

static void memory_budget_hint() {
    MemoryCapability m;
    m.pool_budget_hint_bytes = 4096;
    ASSERT(m.pool_budget_hint_bytes == 4096);
}

// =============================================================================
// BufferCapability tests
// =============================================================================
static void buffer_default_values() {
    BufferCapability b;
    ASSERT(b.buffer_level == CapabilityLevel::None);
    ASSERT(b.max_buffer_frames_ceiling == 0);
}

static void buffer_uses_capability_level() {
    // Verify BufferCapability uses CapabilityLevel, not bool
    BufferCapability b;
    b.buffer_level = CapabilityLevel::Maximum;
    ASSERT(b.buffer_level == CapabilityLevel::Maximum);
    ASSERT(b.buffer_level != CapabilityLevel::None);
}

static void buffer_equality() {
    BufferCapability a{CapabilityLevel::Standard, 1024};
    BufferCapability b{CapabilityLevel::Standard, 1024};
    BufferCapability c{CapabilityLevel::High, 2048};
    ASSERT(a == b);
    ASSERT(a != c);
}

static void buffer_frames_ceiling() {
    BufferCapability b;
    b.max_buffer_frames_ceiling = 8192;
    ASSERT(b.max_buffer_frames_ceiling == 8192);
}

// =============================================================================
// Test runner
// =============================================================================
void run_constraint_contract_tests(ContractTestRunner& runner) {
    runner.run("constraint::strictness_has_all_values", strictness_has_all_values);
    runner.run("constraint::strictness_equality", strictness_equality);
    runner.run("constraint::memory_default_values", memory_default_values);
    runner.run("constraint::memory_uses_capability_level", memory_uses_capability_level);
    runner.run("constraint::memory_equality", memory_equality);
    runner.run("constraint::memory_budget_hint", memory_budget_hint);
    runner.run("constraint::buffer_default_values", buffer_default_values);
    runner.run("constraint::buffer_uses_capability_level", buffer_uses_capability_level);
    runner.run("constraint::buffer_equality", buffer_equality);
    runner.run("constraint::buffer_frames_ceiling", buffer_frames_ceiling);
}
