#include "../../contract_tests_main.h"

#include "kivo/core/contract/observability/metric/metric_kind.hpp"
#include "kivo/core/contract/observability/metric/metric_unit.hpp"
#include "kivo/core/contract/observability/metric/metric_sample.hpp"

using namespace kivo::core::contract::observability;

namespace {

void metric_kind_default_value(ContractTestRunner& runner) {
    runner.run("metric_kind_default_value", []() {
        MetricKind kind{};
        ASSERT(kind == MetricKind::Unknown);
    });
}

void metric_kind_values(ContractTestRunner& runner) {
    runner.run("metric_kind_values", []() {
        ASSERT(static_cast<uint8_t>(MetricKind::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(MetricKind::Counter) == 1);
        ASSERT(static_cast<uint8_t>(MetricKind::Gauge) == 2);
        ASSERT(static_cast<uint8_t>(MetricKind::Histogram) == 3);
        ASSERT(static_cast<uint8_t>(MetricKind::Summary) == 4);
        ASSERT(static_cast<uint8_t>(MetricKind::Timer) == 5);
    });
}

void metric_unit_default_value(ContractTestRunner& runner) {
    runner.run("metric_unit_default_value", []() {
        MetricUnit unit{};
        ASSERT(unit == MetricUnit::Unknown);
    });
}

void metric_unit_values(ContractTestRunner& runner) {
    runner.run("metric_unit_values", []() {
        ASSERT(static_cast<uint8_t>(MetricUnit::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(MetricUnit::None) == 1);
        ASSERT(static_cast<uint8_t>(MetricUnit::Bytes) == 2);
        ASSERT(static_cast<uint8_t>(MetricUnit::Milliseconds) == 3);
        ASSERT(static_cast<uint8_t>(MetricUnit::Seconds) == 4);
        ASSERT(static_cast<uint8_t>(MetricUnit::Percent) == 5);
        ASSERT(static_cast<uint8_t>(MetricUnit::Count) == 6);
        ASSERT(static_cast<uint8_t>(MetricUnit::Ratio) == 7);
    });
}

void metric_sample_default_construction(ContractTestRunner& runner) {
    runner.run("metric_sample_default_construction", []() {
        MetricSample sample{};
        ASSERT(sample.kind == MetricKind::Unknown);
        ASSERT(sample.unit == MetricUnit::Unknown);
        ASSERT(sample.timestamp_ns == 0);
        ASSERT(sample.value == 0.0);
        ASSERT(sample.sample_count == 0);
    });
}

void metric_sample_field_modification(ContractTestRunner& runner) {
    runner.run("metric_sample_field_modification", []() {
        MetricSample sample{};
        sample.kind = MetricKind::Counter;
        sample.unit = MetricUnit::Bytes;
        sample.timestamp_ns = 123456789;
        sample.value = 42.5;
        sample.sample_count = 10;
        ASSERT(sample.kind == MetricKind::Counter);
        ASSERT(sample.unit == MetricUnit::Bytes);
        ASSERT(sample.timestamp_ns == 123456789);
        ASSERT(sample.value == 42.5);
        ASSERT(sample.sample_count == 10);
    });
}

void metric_sample_equality(ContractTestRunner& runner) {
    runner.run("metric_sample_equality", []() {
        MetricSample a{};
        a.kind = MetricKind::Gauge;
        a.unit = MetricUnit::Percent;
        a.timestamp_ns = 100;
        a.value = 75.0;
        a.sample_count = 5;
        MetricSample b{};
        b.kind = MetricKind::Gauge;
        b.unit = MetricUnit::Percent;
        b.timestamp_ns = 100;
        b.value = 75.0;
        b.sample_count = 5;
        MetricSample c{};
        c.kind = MetricKind::Counter;
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void metric_sample_has_all_required_fields(ContractTestRunner& runner) {
    runner.run("metric_sample_has_all_required_fields", []() {
        MetricSample sample{};
        [[maybe_unused]] MetricKind k = sample.kind;
        [[maybe_unused]] MetricUnit u = sample.unit;
        [[maybe_unused]] uint64_t ts = sample.timestamp_ns;
        [[maybe_unused]] double v = sample.value;
        [[maybe_unused]] uint32_t sc = sample.sample_count;
        ASSERT(true);
    });
}

} // namespace

void run_observability_metric_tests(ContractTestRunner& runner) {
    metric_kind_default_value(runner);
    metric_kind_values(runner);
    metric_unit_default_value(runner);
    metric_unit_values(runner);
    metric_sample_default_construction(runner);
    metric_sample_field_modification(runner);
    metric_sample_equality(runner);
    metric_sample_has_all_required_fields(runner);
}