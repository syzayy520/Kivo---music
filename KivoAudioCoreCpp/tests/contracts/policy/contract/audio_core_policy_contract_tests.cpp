#include "../../contract_tests_main.h"
#include "kivo/core/contract/policy/contract/audio_core_policy_contract.hpp"

using namespace kivo::core::contract::policy;

namespace {

void audio_core_policy_contract_default_construction(ContractTestRunner& runner) {
    runner.run("audio_core_policy_contract_default_construction", []() {
        AudioCorePolicyContract c{};
        ASSERT(c.privacy.expose_local_paths == 0);
        ASSERT(c.privacy.expose_user_names == 0);
        ASSERT(c.privacy.expose_file_metadata == 0);
        ASSERT(c.path_exposure.allow_absolute_paths == 0);
        ASSERT(c.path_exposure.allow_home_relative == 0);
        ASSERT(c.path_exposure.sanitize_separators == 0);
        ASSERT(c.identity_sanitization.strip_username == 0);
        ASSERT(c.identity_sanitization.strip_hostname == 0);
        ASSERT(c.identity_sanitization.strip_device_serial == 0);
        ASSERT(c.observation_control.allow_metric_export == 0);
        ASSERT(c.observation_control.allow_trace_export == 0);
        ASSERT(c.observation_control.allow_diagnostic_export == 0);
        ASSERT(c.third_party_notice.require_license_notice == 0);
        ASSERT(c.third_party_notice.require_attribution == 0);
        ASSERT(c.third_party_notice.require_source_offer == 0);
        ASSERT(c.codec_distribution.allow_static_linking == 0);
        ASSERT(c.codec_distribution.allow_dynamic_linking == 0);
        ASSERT(c.codec_distribution.allow_redistribution == 0);
        ASSERT(c.binary_provenance.require_build_signature == 0);
        ASSERT(c.binary_provenance.require_reproducible_build == 0);
        ASSERT(c.binary_provenance.require_source_hash == 0);
        ASSERT(c.commercial_claim.allow_commercial_use == 0);
        ASSERT(c.commercial_claim.require_brand_approval == 0);
        ASSERT(c.commercial_claim.restrict_commercial_distribution == 0);
    });
}

void audio_core_policy_contract_field_modification(ContractTestRunner& runner) {
    runner.run("audio_core_policy_contract_field_modification", []() {
        AudioCorePolicyContract c{};
        c.privacy.expose_local_paths = 1;
        c.path_exposure.allow_absolute_paths = 2;
        c.identity_sanitization.strip_username = 3;
        c.observation_control.allow_metric_export = 4;
        c.third_party_notice.require_license_notice = 5;
        c.codec_distribution.allow_static_linking = 6;
        c.binary_provenance.require_build_signature = 7;
        c.commercial_claim.allow_commercial_use = 8;
        ASSERT(c.privacy.expose_local_paths == 1);
        ASSERT(c.path_exposure.allow_absolute_paths == 2);
        ASSERT(c.identity_sanitization.strip_username == 3);
        ASSERT(c.observation_control.allow_metric_export == 4);
        ASSERT(c.third_party_notice.require_license_notice == 5);
        ASSERT(c.codec_distribution.allow_static_linking == 6);
        ASSERT(c.binary_provenance.require_build_signature == 7);
        ASSERT(c.commercial_claim.allow_commercial_use == 8);
    });
}

void audio_core_policy_contract_equality(ContractTestRunner& runner) {
    runner.run("audio_core_policy_contract_equality", []() {
        AudioCorePolicyContract a{};
        AudioCorePolicyContract b{};
        ASSERT(a == b);
        a.privacy.expose_local_paths = 1;
        ASSERT(!(a == b));
        b.privacy.expose_local_paths = 1;
        ASSERT(a == b);
    });
}

void audio_core_policy_contract_has_all_fields(ContractTestRunner& runner) {
    runner.run("audio_core_policy_contract_has_all_fields", []() {
        AudioCorePolicyContract c{};
        c.privacy = PrivacyBoundaryRule{};
        c.path_exposure = PathExposurePolicy{};
        c.identity_sanitization = SourceIdentitySanitization{};
        c.observation_control = ObservationControlPolicy{};
        c.third_party_notice = ThirdPartyNoticePolicy{};
        c.codec_distribution = CodecDistributionPolicy{};
        c.binary_provenance = BinaryProvenancePolicy{};
        c.commercial_claim = CommercialClaimPolicy{};
        ASSERT(c == AudioCorePolicyContract{});
    });
}

} // anonymous namespace

void run_audio_core_policy_contract_tests(ContractTestRunner& runner) {
    audio_core_policy_contract_default_construction(runner);
    audio_core_policy_contract_field_modification(runner);
    audio_core_policy_contract_equality(runner);
    audio_core_policy_contract_has_all_fields(runner);
}
