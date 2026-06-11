#include "../../contract_tests_main.h"
#include "kivo/core/contract/source/location/source_uri.hpp"
#include "kivo/core/contract/source/location/source_path.hpp"
#include "kivo/core/contract/source/location/source_locator.hpp"

namespace {

void source_uri_default_value_is_zero() {
    kivo::core::contract::SourceUri uri;
    ASSERT(uri.value == 0);
}

void source_uri_equality() {
    kivo::core::contract::SourceUri a{100};
    kivo::core::contract::SourceUri b{100};
    ASSERT(a == b);
}

void source_uri_inequality() {
    kivo::core::contract::SourceUri a{1};
    kivo::core::contract::SourceUri b{2};
    ASSERT(!(a == b));
}

void source_path_default_value_is_zero() {
    kivo::core::contract::SourcePath path;
    ASSERT(path.value == 0);
}

void source_path_equality() {
    kivo::core::contract::SourcePath a{200};
    kivo::core::contract::SourcePath b{200};
    ASSERT(a == b);
}

void source_path_inequality() {
    kivo::core::contract::SourcePath a{1};
    kivo::core::contract::SourcePath b{2};
    ASSERT(!(a == b));
}

void source_locator_default_is_not_specified() {
    kivo::core::contract::SourceLocator locator{};
    ASSERT(locator == kivo::core::contract::SourceLocator::NotSpecified);
}

void source_locator_by_uri() {
    auto locator = kivo::core::contract::SourceLocator::ByUri;
    ASSERT(locator != kivo::core::contract::SourceLocator::NotSpecified);
}

void source_locator_by_path() {
    auto locator = kivo::core::contract::SourceLocator::ByPath;
    ASSERT(locator != kivo::core::contract::SourceLocator::ByUri);
}

void source_locator_equality() {
    auto a = kivo::core::contract::SourceLocator::ByHandle;
    auto b = kivo::core::contract::SourceLocator::ByHandle;
    ASSERT(a == b);
}

} // namespace

void run_source_location_tests(ContractTestRunner& runner) {
    runner.run("source_uri_default_value_is_zero", source_uri_default_value_is_zero);
    runner.run("source_uri_equality", source_uri_equality);
    runner.run("source_uri_inequality", source_uri_inequality);
    runner.run("source_path_default_value_is_zero", source_path_default_value_is_zero);
    runner.run("source_path_equality", source_path_equality);
    runner.run("source_path_inequality", source_path_inequality);
    runner.run("source_locator_default_is_not_specified", source_locator_default_is_not_specified);
    runner.run("source_locator_by_uri", source_locator_by_uri);
    runner.run("source_locator_by_path", source_locator_by_path);
    runner.run("source_locator_equality", source_locator_equality);
}
