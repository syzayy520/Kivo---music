# P0-C-015 Policy Equality Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-015-POLICY-EQUALITY-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C-014 policy boundary structs so their equality operators match the dominant contract style used by the surrounding C++ AudioCore contract families.

The policy family was accepted as value-only, but it currently uses free `friend bool operator==(...)` declarations. This task converts those to member `[[nodiscard]] bool operator==(const Type&) const noexcept = default;` declarations without changing fields, values, include boundaries, file tree, tests, CMake registration, or genealogy allowlists.

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
- No runtime logic.
- No legal text generation.
- No platform, file, installer, third-party binary, source, device, command, or state logic.

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

For every policy struct, replace:

```cpp
friend bool operator==(const Type&, const Type&) noexcept = default;
```

with:

```cpp
[[nodiscard]] bool operator==(const Type&) const noexcept = default;
```

## Invariants

- Type count remains `0 enum + 9 struct = 9 types`.
- File count remains 9 production headers touched.
- Cross-family direct includes remain 0.
- No field names change.
- No field defaults change.
- No namespace changes.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
friend bool operator== removed from policy headers
[[nodiscard]] member operator== present in all 9 policy structs
cross-family direct includes = 0
runtime forbidden tokens = 0
changed files limited to this taskbook + 9 policy headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-015-POLICY-EQUALITY-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
