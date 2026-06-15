#include "performance_evidence_writer.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <vector>

#include "../foundation/latency_distribution.hpp"

namespace kivo::validation::performance {

namespace {

template <typename Value>
void write_array(
    std::ostream& output,
    const std::vector<Value>& values) {
    output << "[";
    for (size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            output << ", ";
        }
        output << values[index];
    }
    output << "]";
}

void write_distribution(
    std::ostream& output,
    const std::vector<uint64_t>& values) {
    const auto distribution = calculate_distribution(values);
    output << "{\"minimum\": " << distribution.minimum
           << ", \"p50\": " << distribution.p50
           << ", \"p95\": " << distribution.p95
           << ", \"maximum\": " << distribution.maximum << "}";
}

[[nodiscard]] std::vector<uint64_t> metric_values(
    const FixturePerformanceEvidence& fixture,
    uint64_t DecodeLatencySample::* member) {
    std::vector<uint64_t> values{};
    values.reserve(fixture.samples.size());
    for (const auto& sample : fixture.samples) {
        values.push_back(sample.*member);
    }
    return values;
}

void write_fixture(
    std::ostream& output,
    const FixturePerformanceEvidence& fixture) {
    output << "    {\n"
           << "      \"fixture_name\": " << std::quoted(fixture.fixture_name)
           << ",\n      \"samples\": [\n";
    for (size_t index = 0; index < fixture.samples.size(); ++index) {
        const auto& sample = fixture.samples[index];
        output << "        {\"open_ns\": " << sample.open_ns
               << ", \"first_block_ns\": " << sample.first_block_ns
               << ", \"seek_ns\": " << sample.seek_ns
               << ", \"full_decode_ns\": " << sample.full_decode_ns
               << ", \"decoded_frames\": " << sample.decoded_frames
               << ", \"decoded_blocks\": " << sample.decoded_blocks << "}";
        output << (index + 1 == fixture.samples.size() ? "\n" : ",\n");
    }
    output << "      ],\n      \"distributions_ns\": {\n";

    output << "        \"open\": ";
    write_distribution(
        output,
        metric_values(fixture, &DecodeLatencySample::open_ns));
    output << ",\n        \"first_block\": ";
    write_distribution(
        output,
        metric_values(fixture, &DecodeLatencySample::first_block_ns));
    output << ",\n        \"seek\": ";
    write_distribution(
        output,
        metric_values(fixture, &DecodeLatencySample::seek_ns));
    output << ",\n        \"full_decode\": ";
    write_distribution(
        output,
        metric_values(fixture, &DecodeLatencySample::full_decode_ns));
    output << "\n      }\n    }";
}

void write_queue(
    std::ostream& output,
    const QueueTimingEvidence& queue) {
    output << "  \"queue\": {\n"
           << "    \"capacity_slots\": " << queue.capacity_slots << ",\n"
           << "    \"observed_peak_used_slots\": "
           << queue.observed_peak_used_slots << ",\n"
           << "    \"successful_pushes\": " << queue.successful_pushes << ",\n"
           << "    \"successful_consumes\": "
           << queue.successful_consumes << ",\n"
           << "    \"occupancy_samples\": ";
    write_array(output, queue.occupancy_samples);
    output << ",\n    \"consumer_interval_ns\": ";
    write_array(output, queue.consumer_interval_ns);
    output << ",\n    \"consumer_interval_distribution_ns\": ";
    write_distribution(output, queue.consumer_interval_ns);
    output << "\n  }\n";
}

} // namespace

void write_performance_evidence(
    const std::filesystem::path& output_path,
    const PerformanceProbeEvidence& evidence) {
    if (evidence.fixtures.empty()) {
        throw std::invalid_argument{"performance evidence has no fixtures"};
    }
    if (const auto parent = output_path.parent_path(); !parent.empty()) {
        std::filesystem::create_directories(parent);
    }
    std::ofstream output{
        output_path,
        std::ios::binary | std::ios::trunc};
    if (!output) {
        throw std::runtime_error{"performance evidence output open failed"};
    }

    output << "{\n"
           << "  \"schema_version\": " << evidence.schema_version << ",\n"
           << "  \"iterations\": " << evidence.iterations << ",\n"
           << "  \"fixtures\": [\n";
    for (size_t index = 0; index < evidence.fixtures.size(); ++index) {
        write_fixture(output, evidence.fixtures[index]);
        output << (index + 1 == evidence.fixtures.size() ? "\n" : ",\n");
    }
    output << "  ],\n";
    write_queue(output, evidence.queue);
    output << "}\n";
    if (!output) {
        throw std::runtime_error{"performance evidence output write failed"};
    }
}

} // namespace kivo::validation::performance
