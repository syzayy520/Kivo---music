#include "../../contract_tests_main.h"
#include "kivo/core/contract/error/domain/error_domain_category.hpp"

using namespace kivo::core::contract;

namespace {

void error_domain_category_default_is_unknown(ContractTestRunner& runner) {
    runner.run("error_domain_category_default_is_unknown", []() {
        ErrorDomainCategory c{};
        ASSERT(c == ErrorDomainCategory::Unknown);
    });
}

void error_domain_category_user(ContractTestRunner& runner) {
    runner.run("error_domain_category_user", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomainCategory::User) == 1);
    });
}

void error_domain_category_system(ContractTestRunner& runner) {
    runner.run("error_domain_category_system", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomainCategory::System) == 2);
    });
}

void error_domain_category_resource(ContractTestRunner& runner) {
    runner.run("error_domain_category_resource", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomainCategory::Resource) == 3);
    });
}

void error_domain_category_codec(ContractTestRunner& runner) {
    runner.run("error_domain_category_codec", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomainCategory::Codec) == 4);
    });
}

void error_domain_category_source(ContractTestRunner& runner) {
    runner.run("error_domain_category_source", []() {
        ASSERT(static_cast<uint8_t>(ErrorDomainCategory::Source) == 5);
    });
}

void error_domain_category_equality(ContractTestRunner& runner) {
    runner.run("error_domain_category_equality", []() {
        ASSERT(ErrorDomainCategory::User == ErrorDomainCategory::User);
        ASSERT(ErrorDomainCategory::Unknown != ErrorDomainCategory::User);
    });
}

void error_domain_category_assignment(ContractTestRunner& runner) {
    runner.run("error_domain_category_assignment", []() {
        ErrorDomainCategory c = ErrorDomainCategory::Codec;
        ASSERT(c == ErrorDomainCategory::Codec);
        c = ErrorDomainCategory::System;
        ASSERT(c == ErrorDomainCategory::System);
    });
}

} // namespace

void run_error_domain_category_tests(ContractTestRunner& runner) {
    error_domain_category_default_is_unknown(runner);
    error_domain_category_user(runner);
    error_domain_category_system(runner);
    error_domain_category_resource(runner);
    error_domain_category_codec(runner);
    error_domain_category_source(runner);
    error_domain_category_equality(runner);
    error_domain_category_assignment(runner);
}