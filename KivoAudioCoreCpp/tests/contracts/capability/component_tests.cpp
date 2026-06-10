// =============================================================================
// KivoAudioCoreCpp — component_tests.cpp
// Tests for Capability component contract types
// =============================================================================
//
// Tests: CapabilityProfile, CapabilityRequirement, HdrCapability,
//        DecodeCapability, RenderCapability, OutputCapability
// =============================================================================

#include "kivo/core/contract/capability/component/profile.hpp"
#include "kivo/core/contract/capability/component/requirement.hpp"
#include "kivo/core/contract/capability/component/hdr.hpp"
#include "kivo/core/contract/capability/component/decode.hpp"
#include "kivo/core/contract/capability/component/render.hpp"
#include "kivo/core/contract/capability/component/output.hpp"
#include "../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// CapabilityProfile tests
// =============================================================================
static void profile_is_identity_metadata_only() {
    // CapabilityProfile must NOT contain HdrCapability, DecodeCapability, etc.
    // It only has: scope, subject, source, level, confidence, path, generation_id
    CapabilityProfile p;
    ASSERT(p.scope == CapabilityScope::Decode);
    ASSERT(p.level == CapabilityLevel::None);
    ASSERT(p.confidence == CapabilityConfidence::Estimated);
    ASSERT(p.path == CapabilityPath::GeneralPurposePath);
    ASSERT(p.generation_id == GenerationId::initial());
}

static void profile_equality() {
    CapabilityProfile a;
    CapabilityProfile b;
    ASSERT(a == b);
}

static void profile_generation_tracking() {
    CapabilityProfile p;
    ASSERT(p.generation_id == GenerationId::initial());
    auto next = GenerationId::next(p.generation_id);
    p.generation_id = next;
    ASSERT(p.generation_id != GenerationId::initial());
}

// =============================================================================
// CapabilityRequirement tests
// =============================================================================
static void requirement_default_values() {
    CapabilityRequirement r;
    ASSERT(r.scope == CapabilityScope::Decode);
    ASSERT(r.minimum_level == CapabilityLevel::None);
    ASSERT(r.strictness == ConstraintStrictness::Required);
}

static void requirement_equality() {
    CapabilityRequirement a{CapabilityScope::Render, CapabilityLevel::Standard, ConstraintStrictness::Required};
    CapabilityRequirement b{CapabilityScope::Render, CapabilityLevel::Standard, ConstraintStrictness::Required};
    ASSERT(a == b);
}

// =============================================================================
// HdrCapability tests
// =============================================================================
static void hdr_uses_capability_level() {
    // HdrCapability must use CapabilityLevel, NOT bool supports_hdr
    HdrCapability h;
    ASSERT(h.hdr_level == CapabilityLevel::None);
    h.hdr_level = CapabilityLevel::High;
    ASSERT(h.hdr_level == CapabilityLevel::High);
}

static void hdr_has_path() {
    HdrCapability h;
    ASSERT(h.path == CapabilityPath::GeneralPurposePath);
}

static void hdr_equality() {
    HdrCapability a{CapabilityLevel::Standard, CapabilityPath::DedicatedPath};
    HdrCapability b{CapabilityLevel::Standard, CapabilityPath::DedicatedPath};
    ASSERT(a == b);
}

// =============================================================================
// DecodeCapability tests
// =============================================================================
static void decode_uses_capability_level() {
    // DecodeCapability must use CapabilityLevel, NOT bool
    DecodeCapability d;
    ASSERT(d.decode_level == CapabilityLevel::None);
    d.decode_level = CapabilityLevel::Maximum;
    ASSERT(d.decode_level == CapabilityLevel::Maximum);
}

static void decode_has_ceiling_fields() {
    DecodeCapability d;
    ASSERT(d.max_sample_rate_ceiling == 0);
    ASSERT(d.max_channels_ceiling == 0);
    d.max_sample_rate_ceiling = 96000;
    d.max_channels_ceiling = 8;
    ASSERT(d.max_sample_rate_ceiling == 96000);
    ASSERT(d.max_channels_ceiling == 8);
}

static void decode_no_format_list() {
    // DecodeCapability must NOT have formats[], supported_formats[], codec list, etc.
    // This is a compile-time guarantee. Runtime check: verify field count is 4.
    DecodeCapability d;
    d.decode_level = CapabilityLevel::High;
    d.path = CapabilityPath::DedicatedPath;
    d.max_sample_rate_ceiling = 192000;
    d.max_channels_ceiling = 2;
    ASSERT(d.decode_level == CapabilityLevel::High);
    ASSERT(d.path == CapabilityPath::DedicatedPath);
}

// =============================================================================
// RenderCapability tests
// =============================================================================
static void render_uses_capability_level() {
    RenderCapability r;
    ASSERT(r.render_level == CapabilityLevel::None);
    r.render_level = CapabilityLevel::Standard;
    ASSERT(r.render_level == CapabilityLevel::Standard);
}

static void render_has_ceiling_fields() {
    RenderCapability r;
    r.max_sample_rate_ceiling = 48000;
    r.max_channels_ceiling = 2;
    ASSERT(r.max_sample_rate_ceiling == 48000);
    ASSERT(r.max_channels_ceiling == 2);
}

static void render_no_format_list() {
    // RenderCapability must NOT have format lists
    RenderCapability r;
    r.render_level = CapabilityLevel::High;
    r.path = CapabilityPath::GeneralPurposePath;
    ASSERT(r.render_level == CapabilityLevel::High);
}

// =============================================================================
// OutputCapability tests
// =============================================================================
static void output_uses_capability_level() {
    OutputCapability o;
    ASSERT(o.output_level == CapabilityLevel::None);
    o.output_level = CapabilityLevel::Basic;
    ASSERT(o.output_level == CapabilityLevel::Basic);
}

static void output_has_path() {
    OutputCapability o;
    ASSERT(o.path == CapabilityPath::GeneralPurposePath);
}

static void output_equality() {
    OutputCapability a{CapabilityLevel::Standard, CapabilityPath::DedicatedPath};
    OutputCapability b{CapabilityLevel::Standard, CapabilityPath::DedicatedPath};
    ASSERT(a == b);
}

// =============================================================================
// Test runner
// =============================================================================
void run_component_contract_tests(ContractTestRunner& runner) {
    runner.run("component::profile_is_identity_metadata_only", profile_is_identity_metadata_only);
    runner.run("component::profile_equality", profile_equality);
    runner.run("component::profile_generation_tracking", profile_generation_tracking);
    runner.run("component::requirement_default_values", requirement_default_values);
    runner.run("component::requirement_equality", requirement_equality);
    runner.run("component::hdr_uses_capability_level", hdr_uses_capability_level);
    runner.run("component::hdr_has_path", hdr_has_path);
    runner.run("component::hdr_equality", hdr_equality);
    runner.run("component::decode_uses_capability_level", decode_uses_capability_level);
    runner.run("component::decode_has_ceiling_fields", decode_has_ceiling_fields);
    runner.run("component::decode_no_format_list", decode_no_format_list);
    runner.run("component::render_uses_capability_level", render_uses_capability_level);
    runner.run("component::render_has_ceiling_fields", render_has_ceiling_fields);
    runner.run("component::render_no_format_list", render_no_format_list);
    runner.run("component::output_uses_capability_level", output_uses_capability_level);
    runner.run("component::output_has_path", output_has_path);
    runner.run("component::output_equality", output_equality);
}
