#include "../../contract_tests_main.h"

#include "kivo/core/contract/observability/diagnostic/diagnostic_snapshot_kind.hpp"
#include "kivo/core/contract/observability/diagnostic/diagnostic_snapshot.hpp"

using namespace kivo::core::contract::observability;

namespace {

void diagnostic_snapshot_kind_default_value(ContractTestRunner& runner) {
    runner.run("diagnostic_snapshot_kind_default_value", []() {
        DiagnosticSnapshotKind kind{};
        ASSERT(kind == DiagnosticSnapshotKind::Unknown);
    });
}

void diagnostic_snapshot_kind_values(ContractTestRunner& runner) {
    runner.run("diagnostic_snapshot_kind_values", []() {
        ASSERT(static_cast<uint8_t>(DiagnosticSnapshotKind::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(DiagnosticSnapshotKind::SystemState) == 1);
        ASSERT(static_cast<uint8_t>(DiagnosticSnapshotKind::Performance) == 2);
        ASSERT(static_cast<uint8_t>(DiagnosticSnapshotKind::ResourceUsage) == 3);
        ASSERT(static_cast<uint8_t>(DiagnosticSnapshotKind::ErrorState) == 4);
        ASSERT(static_cast<uint8_t>(DiagnosticSnapshotKind::Configuration) == 5);
        ASSERT(static_cast<uint8_t>(DiagnosticSnapshotKind::HealthCheck) == 6);
    });
}

void diagnostic_snapshot_default_construction(ContractTestRunner& runner) {
    runner.run("diagnostic_snapshot_default_construction", []() {
        DiagnosticSnapshot snapshot{};
        ASSERT(snapshot.kind == DiagnosticSnapshotKind::Unknown);
        ASSERT(snapshot.timestamp_ns == 0);
        ASSERT(snapshot.snapshot_id == 0);
        ASSERT(snapshot.severity_level == 0);
    });
}

void diagnostic_snapshot_field_modification(ContractTestRunner& runner) {
    runner.run("diagnostic_snapshot_field_modification", []() {
        DiagnosticSnapshot snapshot{};
        snapshot.kind = DiagnosticSnapshotKind::HealthCheck;
        snapshot.timestamp_ns = 123456789;
        snapshot.snapshot_id = 42;
        snapshot.severity_level = 3;
        ASSERT(snapshot.kind == DiagnosticSnapshotKind::HealthCheck);
        ASSERT(snapshot.timestamp_ns == 123456789);
        ASSERT(snapshot.snapshot_id == 42);
        ASSERT(snapshot.severity_level == 3);
    });
}

void diagnostic_snapshot_equality(ContractTestRunner& runner) {
    runner.run("diagnostic_snapshot_equality", []() {
        DiagnosticSnapshot a{};
        a.kind = DiagnosticSnapshotKind::Performance;
        a.timestamp_ns = 100;
        a.snapshot_id = 1;
        a.severity_level = 2;
        DiagnosticSnapshot b{};
        b.kind = DiagnosticSnapshotKind::Performance;
        b.timestamp_ns = 100;
        b.snapshot_id = 1;
        b.severity_level = 2;
        DiagnosticSnapshot c{};
        c.kind = DiagnosticSnapshotKind::ErrorState;
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void diagnostic_snapshot_has_all_required_fields(ContractTestRunner& runner) {
    runner.run("diagnostic_snapshot_has_all_required_fields", []() {
        DiagnosticSnapshot snapshot{};
        [[maybe_unused]] DiagnosticSnapshotKind k = snapshot.kind;
        [[maybe_unused]] uint64_t ts = snapshot.timestamp_ns;
        [[maybe_unused]] uint64_t sid = snapshot.snapshot_id;
        [[maybe_unused]] uint32_t sl = snapshot.severity_level;
        ASSERT(true);
    });
}

} // namespace

void run_observability_diagnostic_tests(ContractTestRunner& runner) {
    diagnostic_snapshot_kind_default_value(runner);
    diagnostic_snapshot_kind_values(runner);
    diagnostic_snapshot_default_construction(runner);
    diagnostic_snapshot_field_modification(runner);
    diagnostic_snapshot_equality(runner);
    diagnostic_snapshot_has_all_required_fields(runner);
}