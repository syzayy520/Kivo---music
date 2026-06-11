# P0-C-012 Command Semantics — Family Design Taskbook

**Task ID:** `KIVO-AUDIO-CORE-P0-C-012-COMMAND-SEMANTICS-FAMILY-DESIGN-TASKBOOK-001`

This is a planning-only taskbook. It is not an implementation ticket.

The next assistant must produce `KIVO-AUDIO-CORE-P0-C-012-FAMILY-DESIGN-DRAFT-001` and wait for Owner approval before creating code.

Required approval phrase:

```text
PLANNING_APPROVED_FOR_IMPLEMENTATION
```

## Scope

P0-C-012 defines command-ordering semantics for rapid user actions and stale command protection.

Use current repository paths:

```text
include/kivo/core/contract/command/
tests/contracts/command/
```

Candidate subfamilies:

```text
identity/
kind/
ordering/
decision/
scenario/
contract/
```

Candidate production files:

```text
identity/command_id.hpp
identity/command_generation_ref.hpp
kind/command_kind.hpp
kind/command_origin.hpp
kind/command_priority.hpp
ordering/command_ordering_policy.hpp
ordering/command_supersession_policy.hpp
ordering/command_coalescing_policy.hpp
ordering/command_ordering_contract.hpp
decision/command_decision.hpp
decision/command_rejection_reason.hpp
scenario/rapid_seek_rule.hpp
scenario/rapid_track_switch_rule.hpp
scenario/pause_during_track_switch_rule.hpp
scenario/seek_during_recovery_rule.hpp
scenario/close_during_drain_rule.hpp
scenario/shutdown_during_write_rule.hpp
scenario/library_scan_during_media_rule.hpp
scenario/source_disconnect_during_seek_rule.hpp
scenario/device_lost_during_flush_rule.hpp
contract/command_semantics_contract.hpp
```

Candidate tests:

```text
tests/contracts/command/identity/command_identity_tests.cpp
tests/contracts/command/kind/command_kind_tests.cpp
tests/contracts/command/ordering/command_ordering_tests.cpp
tests/contracts/command/decision/command_decision_tests.cpp
tests/contracts/command/scenario/command_scenario_tests.cpp
tests/contracts/command/contract/command_semantics_contract_tests.cpp
```

Allowed future modified files after approval:

```text
CMakeLists.txt
tests/contracts/contract_tests_main.cpp
tools/gates/check_contract_genealogy_gate.ps1
```

No runtime implementation. No queue engine. No host or UI wiring. No source or device integration.

## Required planning draft

The planning draft must include Base Gate, Inventory Gate, ODR Gate, natural family tree, file inventory, type matrix, include allowlist, gate update plan, STOP conditions, and implementation readiness verdict.

Final planning classification:

```text
KIVO-AUDIO-CORE-P0-C-012-FAMILY-DESIGN-DRAFT-001
PLANNING_DRAFT_READY_FOR_OWNER_REVIEW
```
