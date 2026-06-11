#include "../../contract_tests_main.h"

#include "kivo/core/contract/observability/event/observation_event_kind.hpp"
#include "kivo/core/contract/observability/event/observation_severity.hpp"

using namespace kivo::core::contract::observability;

namespace {

void observation_event_kind_default_value(ContractTestRunner& runner) {
    runner.run("observation_event_kind_default_value", []() {
        ObservationEventKind kind{};
        ASSERT(kind == ObservationEventKind::Unknown);
    });
}

void observation_event_kind_values(ContractTestRunner& runner) {
    runner.run("observation_event_kind_values", []() {
        ASSERT(static_cast<uint8_t>(ObservationEventKind::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(ObservationEventKind::MetricSample) == 1);
        ASSERT(static_cast<uint8_t>(ObservationEventKind::StateChange) == 2);
        ASSERT(static_cast<uint8_t>(ObservationEventKind::ErrorOccurred) == 3);
        ASSERT(static_cast<uint8_t>(ObservationEventKind::Warning) == 4);
        ASSERT(static_cast<uint8_t>(ObservationEventKind::Info) == 5);
        ASSERT(static_cast<uint8_t>(ObservationEventKind::Debug) == 6);
        ASSERT(static_cast<uint8_t>(ObservationEventKind::Trace) == 7);
    });
}

void observation_severity_default_value(ContractTestRunner& runner) {
    runner.run("observation_severity_default_value", []() {
        ObservationSeverity severity{};
        ASSERT(severity == ObservationSeverity::Unknown);
    });
}

void observation_severity_values(ContractTestRunner& runner) {
    runner.run("observation_severity_values", []() {
        ASSERT(static_cast<uint8_t>(ObservationSeverity::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(ObservationSeverity::Critical) == 1);
        ASSERT(static_cast<uint8_t>(ObservationSeverity::Error) == 2);
        ASSERT(static_cast<uint8_t>(ObservationSeverity::Warning) == 3);
        ASSERT(static_cast<uint8_t>(ObservationSeverity::Info) == 4);
        ASSERT(static_cast<uint8_t>(ObservationSeverity::Debug) == 5);
        ASSERT(static_cast<uint8_t>(ObservationSeverity::Trace) == 6);
    });
}

void observation_event_kind_equality(ContractTestRunner& runner) {
    runner.run("observation_event_kind_equality", []() {
        ObservationEventKind a{ObservationEventKind::Info};
        ObservationEventKind b{ObservationEventKind::Info};
        ASSERT(a == b);

        ObservationEventKind c{ObservationEventKind::Debug};
        ASSERT(!(a == c));
    });
}

void observation_severity_equality(ContractTestRunner& runner) {
    runner.run("observation_severity_equality", []() {
        ObservationSeverity a{ObservationSeverity::Warning};
        ObservationSeverity b{ObservationSeverity::Warning};
        ASSERT(a == b);

        ObservationSeverity c{ObservationSeverity::Error};
        ASSERT(!(a == c));
    });
}

} // namespace

void run_observability_event_tests(ContractTestRunner& runner) {
    observation_event_kind_default_value(runner);
    observation_event_kind_values(runner);
    observation_severity_default_value(runner);
    observation_severity_values(runner);
    observation_event_kind_equality(runner);
    observation_severity_equality(runner);
}
