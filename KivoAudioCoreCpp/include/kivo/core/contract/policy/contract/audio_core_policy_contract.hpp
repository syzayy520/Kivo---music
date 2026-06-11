#pragma once

#include "../privacy/privacy_boundary_rule.hpp"
#include "../privacy/path_exposure_policy.hpp"
#include "../privacy/source_identity_sanitization.hpp"
#include "../privacy/observation_control_policy.hpp"
#include "../distribution/third_party_notice_policy.hpp"
#include "../distribution/codec_distribution_policy.hpp"
#include "../distribution/binary_provenance_policy.hpp"
#include "../distribution/commercial_claim_policy.hpp"

namespace kivo::core::contract::policy {

struct AudioCorePolicyContract {
    PrivacyBoundaryRule privacy{};
    PathExposurePolicy path_exposure{};
    SourceIdentitySanitization identity_sanitization{};
    ObservationControlPolicy observation_control{};
    ThirdPartyNoticePolicy third_party_notice{};
    CodecDistributionPolicy codec_distribution{};
    BinaryProvenancePolicy binary_provenance{};
    CommercialClaimPolicy commercial_claim{};

    [[nodiscard]] constexpr bool operator==(const AudioCorePolicyContract&) const noexcept = default;
};

} // namespace kivo::core::contract::policy
