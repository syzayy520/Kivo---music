#include "../../contract_tests_main.h"

#include "kivo/core/contract/observability/contract/observability_contract.hpp"

using namespace kivo::core::contract::observability;

namespace {

void observability_contract_default_construction(ContractTestRunner& runner) {
    runner.run("observability_contract_default_construction", []() {
        ObservabilityContract contract{};
        ASSERT(contract.trace_buffer_policy == TraceBufferPolicy::Unknown);
        ASSERT(contract.min_severity == ObservationSeverity::Unknown);
        ASSERT(contract.enable_metrics == false);
        ASSERT(contract.enable_tracing == false);
        ASSERT(contract.enable_diagnostics == false);
        ASSERT(contract.max_metric_samples == 0);
        ASSERT(contract.max_trace_records == 0);
        ASSERT(contract.max_diagnostic_snapshots == 0);
    });
}

void observability_contract_field_modification(ContractTestRunner& runner) {
    runner.run("observability_contract_field_modification", []() {
        ObservabilityContract contract{};
        contract.trace_buffer_policy = TraceBufferPolicy::DropOldest;
        contract.min_severity = ObservationSeverity::Warning;
        contract.enable_metrics = true;
        contract.enable_tracing = true;
        contract.enable_diagnostics = false;
        contract.max_metric_samples = 1000;
        contract.max_trace_records = 5000;
        contract.max_diagnostic_snapshots = 100;
        ASSERT(contract.trace_buffer_policy == TraceBufferPolicy::DropOldest);
        ASSERT(contract.min_severity == ObservationSeverity::Warning);
        ASSERT(contract.enable_metrics == true);
        ASSERT(contract.enable_tracing == true);
        ASSERT(contract.enable_diagnostics == false);
        ASSERT(contract.max_metric_samples == 1000);
        ASSERT(contract.max_trace_records == 5000);
        ASSERT(contract.max_diagnostic_snapshots == 100);
    });
}

void observability_contract_equality(ContractTestRunner& runner) {
    runner.run("observability_contract_equality", []() {
        ObservabilityContract a{};
        a.trace_buffer_policy = TraceBufferPolicy::FixedSize;
        a.min_severity = ObservationSeverity::Info;
        a.enable_metrics = true;
        a.enable_tracing = false;
        a.enable_diagnostics = true;
        a.max_metric_samples = 500;
        a.max_trace_records = 2000;
        a.max_diagnostic_snapshots = 50;
        ObservabilityContract b{};
        b.trace_buffer_policy = TraceBufferPolicy::FixedSize;
        b.min_severity = ObservationSeverity::Info;
        b.enable_metrics = true;
        b.enable_tracing = false;
        b.enable_diagnostics = true;
        b.max_metric_samples = 500;
        b.max_trace_records = 2000;
        b.max_diagnostic_snapshots = 50;
        ObservabilityContract c{};
        c.trace_buffer_policy = TraceBufferPolicy::Unbounded;
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void observability_contract_has_all_required_fields(ContractTestRunner& runner) {
    runner.run("observability_contract_has_all_required_fields", []() {
        ObservabilityContract contract{};
        [[maybe_unused]] TraceBufferPolicy tbp = contract.trace_buffer_policy;
        [[maybe_unused]] ObservationSeverity ms = contract.min_severity;
        [[maybe_unused]] bool em = contract.enable_metrics;
        [[maybe_unused]] bool et = contract.enable_tracing;
        [[maybe_unused]] bool ed = contract.enable_diagnostics;
        [[maybe_unused]] uint32_t mms = contract.max_metric_samples;
        [[maybe_unused]] uint32_t mtr = contract.max_trace_records;
        [[maybe_unused]] uint32_t mds = contract.max_diagnostic_snapshots;
        ASSERT(true);
    });
}

} // namespace

void run_observability_contract_tests(ContractTestRunner& runner) {
    observability_contract_default_construction(runner);
    observability_contract_field_modification(runner);
    observability_contract_equality(runner);
    observability_contract_has_all_required_fields(runner);
}
