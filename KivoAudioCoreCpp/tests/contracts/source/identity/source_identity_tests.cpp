#include "../../contract_tests_main.h"
#include "kivo/core/contract/source/identity/source_identity.hpp"
#include "kivo/core/contract/source/identity/source_generation.hpp"
#include "kivo/core/contract/source/identity/source_location_ref.hpp"

namespace {

void source_identity_default_value_is_zero() {
    kivo::core::contract::SourceIdentity id;
    ASSERT(id.value == 0);
}

void source_identity_equality() {
    kivo::core::contract::SourceIdentity a{42};
    kivo::core::contract::SourceIdentity b{42};
    ASSERT(a == b);
}

void source_identity_inequality() {
    kivo::core::contract::SourceIdentity a{1};
    kivo::core::contract::SourceIdentity b{2};
    ASSERT(!(a == b));
}

void source_identity_copy() {
    kivo::core::contract::SourceIdentity a{99};
    kivo::core::contract::SourceIdentity b = a;
    ASSERT(a == b);
    ASSERT(b.value == 99);
}

void source_generation_default_is_initial() {
    kivo::core::contract::SourceGeneration gen;
    ASSERT(gen.value() == kivo::core::contract::GenerationId::initial().value());
}

void source_generation_equality() {
    kivo::core::contract::SourceGeneration a;
    kivo::core::contract::SourceGeneration b;
    ASSERT(a == b);
}

void source_generation_inequality() {
    kivo::core::contract::SourceGeneration a;
    kivo::core::contract::SourceGeneration b{kivo::core::contract::GenerationId::next(a.id)};
    ASSERT(!(a == b));
}

void source_generation_value_accessor() {
    kivo::core::contract::SourceGeneration gen;
    ASSERT(gen.value() == gen.id.value());
}

void source_location_ref_default_value_is_zero() {
    kivo::core::contract::SourceLocationRef ref;
    ASSERT(ref.value == 0);
}

void source_location_ref_equality() {
    kivo::core::contract::SourceLocationRef a{7};
    kivo::core::contract::SourceLocationRef b{7};
    ASSERT(a == b);
}

} // namespace

void run_source_identity_tests(ContractTestRunner& runner) {
    runner.run("source_identity_default_value_is_zero", source_identity_default_value_is_zero);
    runner.run("source_identity_equality", source_identity_equality);
    runner.run("source_identity_inequality", source_identity_inequality);
    runner.run("source_identity_copy", source_identity_copy);
    runner.run("source_generation_default_is_initial", source_generation_default_is_initial);
    runner.run("source_generation_equality", source_generation_equality);
    runner.run("source_generation_inequality", source_generation_inequality);
    runner.run("source_generation_value_accessor", source_generation_value_accessor);
    runner.run("source_location_ref_default_value_is_zero", source_location_ref_default_value_is_zero);
    runner.run("source_location_ref_equality", source_location_ref_equality);
}
