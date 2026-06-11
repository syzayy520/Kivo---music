#include "../../contract_tests_main.h"

#include "kivo/core/contract/observability/trace/trace_event_kind.hpp"
#include "kivo/core/contract/observability/trace/trace_buffer_policy.hpp"
#include "kivo/core/contract/observability/trace/trace_record.hpp"

using namespace kivo::core::contract::observability;

namespace {

void trace_event_kind_default_value(ContractTestRunner& runner) {
    runner.run("trace_event_kind_default_value", []() {
        TraceEventKind kind{};
        ASSERT(kind == TraceEventKind::Unknown);
    });
}

void trace_event_kind_values(ContractTestRunner& runner) {
    runner.run("trace_event_kind_values", []() {
        ASSERT(static_cast<uint8_t>(TraceEventKind::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(TraceEventKind::SpanStart) == 1);
        ASSERT(static_cast<uint8_t>(TraceEventKind::SpanEnd) == 2);
        ASSERT(static_cast<uint8_t>(TraceEventKind::Event) == 3);
        ASSERT(static_cast<uint8_t>(TraceEventKind::Log) == 4);
        ASSERT(static_cast<uint8_t>(TraceEventKind::Metric) == 5);
        ASSERT(static_cast<uint8_t>(TraceEventKind::Error) == 6);
        ASSERT(static_cast<uint8_t>(TraceEventKind::Debug) == 7);
    });
}

void trace_buffer_policy_default_value(ContractTestRunner& runner) {
    runner.run("trace_buffer_policy_default_value", []() {
        TraceBufferPolicy policy{};
        ASSERT(policy == TraceBufferPolicy::Unknown);
    });
}

void trace_buffer_policy_values(ContractTestRunner& runner) {
    runner.run("trace_buffer_policy_values", []() {
        ASSERT(static_cast<uint8_t>(TraceBufferPolicy::Unknown) == 0);
        ASSERT(static_cast<uint8_t>(TraceBufferPolicy::DropOldest) == 1);
        ASSERT(static_cast<uint8_t>(TraceBufferPolicy::DropNewest) == 2);
        ASSERT(static_cast<uint8_t>(TraceBufferPolicy::Block) == 3);
        ASSERT(static_cast<uint8_t>(TraceBufferPolicy::FixedSize) == 4);
        ASSERT(static_cast<uint8_t>(TraceBufferPolicy::Unbounded) == 5);
    });
}

void trace_record_default_construction(ContractTestRunner& runner) {
    runner.run("trace_record_default_construction", []() {
        TraceRecord record{};
        ASSERT(record.kind == TraceEventKind::Unknown);
        ASSERT(record.timestamp_ns == 0);
        ASSERT(record.duration_ns == 0);
        ASSERT(record.trace_id == 0);
        ASSERT(record.span_id == 0);
        ASSERT(record.parent_span_id == 0);
    });
}

void trace_record_field_modification(ContractTestRunner& runner) {
    runner.run("trace_record_field_modification", []() {
        TraceRecord record{};
        record.kind = TraceEventKind::SpanStart;
        record.timestamp_ns = 123456789;
        record.duration_ns = 1000;
        record.trace_id = 42;
        record.span_id = 1;
        record.parent_span_id = 0;
        ASSERT(record.kind == TraceEventKind::SpanStart);
        ASSERT(record.timestamp_ns == 123456789);
        ASSERT(record.duration_ns == 1000);
        ASSERT(record.trace_id == 42);
        ASSERT(record.span_id == 1);
        ASSERT(record.parent_span_id == 0);
    });
}

void trace_record_equality(ContractTestRunner& runner) {
    runner.run("trace_record_equality", []() {
        TraceRecord a{};
        a.kind = TraceEventKind::Event;
        a.timestamp_ns = 100;
        a.duration_ns = 50;
        a.trace_id = 1;
        a.span_id = 2;
        a.parent_span_id = 0;
        TraceRecord b{};
        b.kind = TraceEventKind::Event;
        b.timestamp_ns = 100;
        b.duration_ns = 50;
        b.trace_id = 1;
        b.span_id = 2;
        b.parent_span_id = 0;
        TraceRecord c{};
        c.kind = TraceEventKind::Log;
        ASSERT(a == b);
        ASSERT(!(a == c));
    });
}

void trace_record_has_all_required_fields(ContractTestRunner& runner) {
    runner.run("trace_record_has_all_required_fields", []() {
        TraceRecord record{};
        [[maybe_unused]] TraceEventKind k = record.kind;
        [[maybe_unused]] uint64_t ts = record.timestamp_ns;
        [[maybe_unused]] uint64_t dur = record.duration_ns;
        [[maybe_unused]] uint64_t tid = record.trace_id;
        [[maybe_unused]] uint64_t sid = record.span_id;
        [[maybe_unused]] uint64_t psid = record.parent_span_id;
        ASSERT(true);
    });
}

} // namespace

void run_observability_trace_tests(ContractTestRunner& runner) {
    trace_event_kind_default_value(runner);
    trace_event_kind_values(runner);
    trace_buffer_policy_default_value(runner);
    trace_buffer_policy_values(runner);
    trace_record_default_construction(runner);
    trace_record_field_modification(runner);
    trace_record_equality(runner);
    trace_record_has_all_required_fields(runner);
}