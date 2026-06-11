# P0-C-024 Observability Constexpr Equality Normalization — Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-024-OBSERVABILITY-CONSTEXPR-EQUALITY-NORMALIZATION-TASKBOOK-001`

Implementation allowed by Owner authorization.

## Goal

Normalize the P0-C-013 observability value structs so equality operators are explicitly `constexpr`, matching the pure value-contract style now used by neighboring error, source, command, CUE, identity, and generation families.

The observability family is already value-only. This task only annotates existing member `operator==` declarations. It must not change fields, values, include boundaries, file tree, tests, CMake registration, or genealogy allowlists.

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
- No enum value changes.
- No field name changes.
- No field default changes.
- No include path changes.
- No namespace changes.
- No runtime logic.
- No logger, telemetry sink, metric collector, trace writer, diagnostic exporter, ring buffer, thread, platform, source reader, or recovery execution logic.

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

For the four selected observability structs, replace:

```cpp
[[nodiscard]] bool operator==(const Type&) const noexcept = default;
```

with:

```cpp
[[nodiscard]] constexpr bool operator==(const Type&) const noexcept = default;
```

## Invariants

- Observability type count remains unchanged.
- File count remains 4 production headers touched.
- Cross-family direct includes remain unchanged.
- No field names change.
- No field defaults change.
- No include path changes.
- No namespace changes.
- No test source changes.

## Static self-check requirements

After implementation, verify:

```text
all 4 selected observability structs contain [[nodiscard]] constexpr member operator==
non-constexpr member operator== removed from those 4 structs
cross-family direct includes unchanged
runtime forbidden tokens = 0
changed files limited to this taskbook + 4 observability headers
```

## Final classification

```text
KIVO-AUDIO-CORE-P0-C-024-OBSERVABILITY-CONSTEXPR-EQUALITY-NORMALIZATION
IMPLEMENTED_STATIC_SELF_CHECK_PASS_READY_FOR_LOCAL_VALIDATION
```
