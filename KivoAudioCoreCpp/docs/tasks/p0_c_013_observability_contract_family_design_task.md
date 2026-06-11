# P0-C-013 Observability Contract — Family Design Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-013-OBSERVABILITY-CONTRACT-FAMILY-DESIGN-TASKBOOK-001`

Planning only. No implementation.

Owner approval required before implementation:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Goal

Define value-only observability contract types for metric identifiers, event identifiers, event categories, event severity, retention policy, and aggregate observability settings.

Use repository paths:

```text
include/kivo/core/contract/observability/
tests/contracts/observability/
```

## Candidate tree

```text
observability/
  metric/
    metric_id.hpp
    metric_unit.hpp
    metric_sample_kind.hpp
    metric_contract.hpp
  event/
    event_id.hpp
    event_category.hpp
    event_severity.hpp
    event_contract.hpp
  policy/
    observation_retention_policy.hpp
    observation_level.hpp
  contract/
    observability_contract.hpp
```

Candidate tests:

```text
tests/contracts/observability/metric/metric_contract_tests.cpp
tests/contracts/observability/event/event_contract_tests.cpp
tests/contracts/observability/policy/observation_policy_tests.cpp
tests/contracts/observability/contract/observability_contract_tests.cpp
```

Candidate count:

```text
Production headers: 11
Test files: 4
Modified files: 3
Total touched: 18
```

## Rules

All structs must be value-only: fields, default initializers, and `operator==` only.

Default include is `<cstdint>`. Composite files may include sibling observability headers only.

Implementation must not add backend code, UI code, storage code, or runtime behavior.

## Required planning draft

The next assistant must output Base Gate, Inventory Gate, ODR Gate, natural family tree, file inventory, type matrix, include allowlist, gate update plan, STOP conditions, and implementation readiness verdict.

Final planning classification:

```text
KIVO-AUDIO-CORE-P0-C-013-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
