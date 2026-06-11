# P0-C-025 Policy Constexpr Equality Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-025-POLICY-CONSTEXPR-EQUALITY-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C-014 policy value structs so equality operators are explicitly `constexpr`, matching the pure value-contract style now used by neighboring observability, error, source, command, CUE, identity, and generation families.

The policy family is already value-only. This task only annotates existing member `operator==` declarations. It must not change fields, values, include boundaries, file tree, tests, CMake registration, or genealogy allowlists.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/policy/
```

Allowed production headers:

```text
include/kivo/core/contract/policy/privacy/privacy_boundary_rule.hpp
include/kivo/core/contract/policy/privacy/path_exposure_policy.hpp
include/kivo/core/contract/policy/privacy/source_identity_sanitization.hpp
include/kivo/core/contract/policy/privacy/observation_control_policy.hpp
include/kivo/core/contract/policy/distribution/third_party_notice_policy.hpp
include/kivo/core/contract/policy/distribution/codec_distribution_policy.hpp
include/kivo/core/contract/policy/distribution/binary_provenance_policy.hpp
include/kivo/core/contract/policy/distribution/commercial_claim_policy.hpp
include/kivo/core/contract/policy/contract/audio_core_policy_contract.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No field name changes.
- No field default changes.
- No include path changes.
- No namespace changes.
- No runtime logic.
- No privacy scanner, path resolver, source identity exporter, notice generator, codec distributor, binary provenance verifier, legal text generator, platform, thread, or telemetry execution logic.

## Natural family tree

Existing tree is retained exactly:

```text
policy/
  privacy/
    privacy_boundary_rule.hpp
    path_exposure_policy.hpp
    source_identity_sanitization.hpp
    observation_control_policy.hpp
  distribution/
    third_party_notice_policy.hpp
    codec_distribution_policy.hpp
    binary_provenance_policy.hpp
    commercial_claim_policy.hpp
  contract/
    audio_core_policy_contract.hpp
```

## Required transformation

For all 9 policy structs, replace:

```cpp
[[nodiscard]] bool operator==(const Type&) const noexcept = default;
```

with:

```cpp
[[nodiscard]] constexpr bool operator==(const Type&) const noexcept = default;
```

## Invariants

- Policy type count remains `0 enum + 9 struct = 9 types`.
- File count remains 9 production headers touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
all 9 policy structs contain [[nodiscard]] constexpr member operator==
non-constexpr member operator== removed from those 9 structs
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 9 policy headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-025-POLICY-CONSTEXPR-EQUALITY-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
