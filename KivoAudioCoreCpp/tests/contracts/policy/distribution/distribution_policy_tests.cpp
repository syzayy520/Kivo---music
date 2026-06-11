#include "../../contract_tests_main.h"
#include "kivo/core/contract/policy/distribution/third_party_notice_policy.hpp"
#include "kivo/core/contract/policy/distribution/codec_distribution_policy.hpp"
#include "kivo/core/contract/policy/distribution/binary_provenance_policy.hpp"
#include "kivo/core/contract/policy/distribution/commercial_claim_policy.hpp"

using namespace kivo::core::contract::policy;

namespace {

void third_party_notice_policy_default_construction(ContractTestRunner& runner) {
    runner.run("third_party_notice_policy_default_construction", []() {
        ThirdPartyNoticePolicy policy{};
        ASSERT(policy.require_license_notice == 0);
        ASSERT(policy.require_attribution == 0);
        ASSERT(policy.require_source_offer == 0);
    });
}

void third_party_notice_policy_field_modification(ContractTestRunner& runner) {
    runner.run("third_party_notice_policy_field_modification", []() {
        ThirdPartyNoticePolicy policy{};
        policy.require_license_notice = 1;
        policy.require_attribution = 2;
        policy.require_source_offer = 3;
        ASSERT(policy.require_license_notice == 1);
        ASSERT(policy.require_attribution == 2);
        ASSERT(policy.require_source_offer == 3);
    });
}

void third_party_notice_policy_equality(ContractTestRunner& runner) {
    runner.run("third_party_notice_policy_equality", []() {
        ThirdPartyNoticePolicy a{};
        ThirdPartyNoticePolicy b{};
        ASSERT(a == b);
        a.require_license_notice = 1;
        ASSERT(!(a == b));
        b.require_license_notice = 1;
        ASSERT(a == b);
    });
}

void codec_distribution_policy_default_construction(ContractTestRunner& runner) {
    runner.run("codec_distribution_policy_default_construction", []() {
        CodecDistributionPolicy policy{};
        ASSERT(policy.allow_static_linking == 0);
        ASSERT(policy.allow_dynamic_linking == 0);
        ASSERT(policy.allow_redistribution == 0);
    });
}

void codec_distribution_policy_field_modification(ContractTestRunner& runner) {
    runner.run("codec_distribution_policy_field_modification", []() {
        CodecDistributionPolicy policy{};
        policy.allow_static_linking = 1;
        policy.allow_dynamic_linking = 2;
        policy.allow_redistribution = 3;
        ASSERT(policy.allow_static_linking == 1);
        ASSERT(policy.allow_dynamic_linking == 2);
        ASSERT(policy.allow_redistribution == 3);
    });
}

void codec_distribution_policy_equality(ContractTestRunner& runner) {
    runner.run("codec_distribution_policy_equality", []() {
        CodecDistributionPolicy a{};
        CodecDistributionPolicy b{};
        ASSERT(a == b);
        a.allow_static_linking = 1;
        ASSERT(!(a == b));
        b.allow_static_linking = 1;
        ASSERT(a == b);
    });
}

void binary_provenance_policy_default_construction(ContractTestRunner& runner) {
    runner.run("binary_provenance_policy_default_construction", []() {
        BinaryProvenancePolicy policy{};
        ASSERT(policy.require_build_signature == 0);
        ASSERT(policy.require_reproducible_build == 0);
        ASSERT(policy.require_source_hash == 0);
    });
}

void binary_provenance_policy_field_modification(ContractTestRunner& runner) {
    runner.run("binary_provenance_policy_field_modification", []() {
        BinaryProvenancePolicy policy{};
        policy.require_build_signature = 1;
        policy.require_reproducible_build = 2;
        policy.require_source_hash = 3;
        ASSERT(policy.require_build_signature == 1);
        ASSERT(policy.require_reproducible_build == 2);
        ASSERT(policy.require_source_hash == 3);
    });
}

void binary_provenance_policy_equality(ContractTestRunner& runner) {
    runner.run("binary_provenance_policy_equality", []() {
        BinaryProvenancePolicy a{};
        BinaryProvenancePolicy b{};
        ASSERT(a == b);
        a.require_build_signature = 1;
        ASSERT(!(a == b));
        b.require_build_signature = 1;
        ASSERT(a == b);
    });
}

void commercial_claim_policy_default_construction(ContractTestRunner& runner) {
    runner.run("commercial_claim_policy_default_construction", []() {
        CommercialClaimPolicy policy{};
        ASSERT(policy.allow_commercial_use == 0);
        ASSERT(policy.require_brand_approval == 0);
        ASSERT(policy.restrict_commercial_distribution == 0);
    });
}

void commercial_claim_policy_field_modification(ContractTestRunner& runner) {
    runner.run("commercial_claim_policy_field_modification", []() {
        CommercialClaimPolicy policy{};
        policy.allow_commercial_use = 1;
        policy.require_brand_approval = 2;
        policy.restrict_commercial_distribution = 3;
        ASSERT(policy.allow_commercial_use == 1);
        ASSERT(policy.require_brand_approval == 2);
        ASSERT(policy.restrict_commercial_distribution == 3);
    });
}

void commercial_claim_policy_equality(ContractTestRunner& runner) {
    runner.run("commercial_claim_policy_equality", []() {
        CommercialClaimPolicy a{};
        CommercialClaimPolicy b{};
        ASSERT(a == b);
        a.allow_commercial_use = 1;
        ASSERT(!(a == b));
        b.allow_commercial_use = 1;
        ASSERT(a == b);
    });
}

} // anonymous namespace

void run_distribution_policy_tests(ContractTestRunner& runner) {
    third_party_notice_policy_default_construction(runner);
    third_party_notice_policy_field_modification(runner);
    third_party_notice_policy_equality(runner);
    codec_distribution_policy_default_construction(runner);
    codec_distribution_policy_field_modification(runner);
    codec_distribution_policy_equality(runner);
    binary_provenance_policy_default_construction(runner);
    binary_provenance_policy_field_modification(runner);
    binary_provenance_policy_equality(runner);
    commercial_claim_policy_default_construction(runner);
    commercial_claim_policy_field_modification(runner);
    commercial_claim_policy_equality(runner);
}
