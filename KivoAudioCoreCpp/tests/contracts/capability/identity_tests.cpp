// =============================================================================
// KivoAudioCoreCpp — identity_tests.cpp
// Tests for Capability identity contract types
// =============================================================================
//
// Tests: CapabilityScope, CapabilitySubject, CapabilitySource
// =============================================================================

#include "kivo/core/contract/capability/identity/scope.hpp"
#include "kivo/core/contract/capability/identity/subject.hpp"
#include "kivo/core/contract/capability/identity/source.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// CapabilityScope tests
// =============================================================================
static void scope_has_all_values() {
    // Verify all 8 scope values exist
    auto decode = CapabilityScope::Decode;
    auto render = CapabilityScope::Render;
    auto convert = CapabilityScope::Convert;
    auto output = CapabilityScope::Output;
    auto source = CapabilityScope::Source;
    auto clock = CapabilityScope::Clock;
    auto memory = CapabilityScope::Memory;
    auto thread = CapabilityScope::Thread;
    ASSERT(decode != render);
    ASSERT(convert != output);
    ASSERT(source != clock);
    ASSERT(memory != thread);
}

static void scope_equality() {
    ASSERT(CapabilityScope::Decode == CapabilityScope::Decode);
    ASSERT(CapabilityScope::Decode != CapabilityScope::Render);
}

// =============================================================================
// CapabilitySubject tests
// =============================================================================
static void subject_default_kind() {
    CapabilitySubject s;
    ASSERT(s.kind == SubjectKind::System);
}

static void subject_equality() {
    CapabilitySubject a{SubjectKind::Device};
    CapabilitySubject b{SubjectKind::Device};
    CapabilitySubject c{SubjectKind::Codec};
    ASSERT(a == b);
    ASSERT(a != c);
}

static void subject_kind_values() {
    ASSERT(SubjectKind::Device != SubjectKind::Codec);
    ASSERT(SubjectKind::Pipeline != SubjectKind::System);
}

// =============================================================================
// CapabilitySource tests
// =============================================================================
static void source_default_origin() {
    CapabilitySource s;
    ASSERT(s.origin == SourceOrigin::System);
}

static void source_equality() {
    CapabilitySource a{SourceOrigin::Device};
    CapabilitySource b{SourceOrigin::Device};
    CapabilitySource c{SourceOrigin::User};
    ASSERT(a == b);
    ASSERT(a != c);
}

static void source_origin_values() {
    ASSERT(SourceOrigin::Device != SourceOrigin::Codec);
    ASSERT(SourceOrigin::Pipeline != SourceOrigin::System);
    ASSERT(SourceOrigin::User != SourceOrigin::Device);
}

// =============================================================================
// Test runner
// =============================================================================
void run_identity_contract_tests(ContractTestRunner& runner) {
    runner.run("identity::scope_has_all_values", scope_has_all_values);
    runner.run("identity::scope_equality", scope_equality);
    runner.run("identity::subject_default_kind", subject_default_kind);
    runner.run("identity::subject_equality", subject_equality);
    runner.run("identity::subject_kind_values", subject_kind_values);
    runner.run("identity::source_default_origin", source_default_origin);
    runner.run("identity::source_equality", source_equality);
    runner.run("identity::source_origin_values", source_origin_values);
}
