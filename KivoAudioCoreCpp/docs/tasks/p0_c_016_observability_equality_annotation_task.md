# P0-C-016 Observability Equality Annotation — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-016-OBSERVABILITY-EQUALITY-ANNOTATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C-013 observability value structs so equality operators explicitly use `[[nodiscard]]`, matching the stricter member equality style now used by the policy family and nearby value-contract families.

The observability family is already value-only. This task only annotates existing member `operator==` declarations. It must not change fields, values, includes, file tree, tests, CMake registration, or genealogy allowlists.

## Scope

Update only existing production headers under:

```text
include/kivo/core/contract/observability/
```

Allowed production headers:

```text
include/kivo/core/contract/observability/metric/metric_sample.hpp
include/kivo/core/contract/observability/trace/trace_record.hpp
include/kivo/core/contract/observability/diagnostic/diagnostic_snapshot.hpp
include/kivo/core/contract/observability/contract/observability_contract.hpp
```

## Non-goals

- No new production headers.
- No new tests.
- No CMake changes.
- No runner changes.
- No genealogy gate changes.
- No enum changes.
- No field changes.
- No runtime logic.
- No logger, telemetry sink, metrics collector, trace writer, ring buffer, platform, source, device, command, or state logic.

## Natural family tree

Existing tree is retained exactly:

```text
observability/
  event/
    observation_event_kind.hpp
    observation_severity.hpp
  metric/
    metric_kind.hpp
    metric_unit.hpp
    metric_sample.hpp
  trace/
    trace_event_kind.hpp
    trace_record.hpp
    trace_buffer_policy.hpp
  diagnostic/
    diagnostic_snapshot_kind.hpp
    diagnostic_snapshot.hpp
  contract/
    observability_contract.hpp
```

## Required transformation

For the four observability structs, replace:

```cpp
bool operator==(const Type&) const noexcept = default;
```

with:

```cpp
[[nodiscard]] bool operator==(const Type&) const noexcept = default;
```

## Invariants

- Observability type count remains `7 enum + 4 struct = 11 types`.
- File count remains 4 production headers touched.
- Cross-family direct includes remain 0.
- No field names change.
- No field defaults change.
- No namespace changes.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
all 4 observability structs contain [[nodiscard]] member operator==
plain non-nodiscard member operator== removed from those 4 structs
cross-family direct includes = 0
runtime forbidden tokens = 0
changed files limited to this taskbook + 4 observability headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-016-OBSERVABILITY-EQUALITY-ANNOTATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
