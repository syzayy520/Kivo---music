// =============================================================================
// KivoAudioCoreCpp — sample_format_tests.cpp
// Tests for SampleFormat contract type
// =============================================================================

#include "kivo/core/contract/format/descriptor/sample_format.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void sample_format_values_are_distinct() {
    ASSERT(static_cast<uint8_t>(SampleFormat::Unknown) == 0);
    ASSERT(SampleFormat::Int16 != SampleFormat::Int24);
    ASSERT(SampleFormat::Float32 != SampleFormat::Float64);
    ASSERT(SampleFormat::Int32 != SampleFormat::Float32);
}

static void valid_bits_of_unknown_is_zero() {
    ASSERT(sample_format_valid_bits(SampleFormat::Unknown) == 0);
}

static void valid_bits_of_int16_is_16() {
    ASSERT(sample_format_valid_bits(SampleFormat::Int16) == 16);
}

static void valid_bits_of_int24_is_24() {
    ASSERT(sample_format_valid_bits(SampleFormat::Int24) == 24);
}

static void container_bits_of_int24_is_32() {
    ASSERT(sample_format_container_bits(SampleFormat::Int24) == 32);
}

static void int_formats_are_integer() {
    ASSERT(sample_format_is_integer(SampleFormat::Int16));
    ASSERT(sample_format_is_integer(SampleFormat::Int24));
    ASSERT(sample_format_is_integer(SampleFormat::Int32));
}

static void float_formats_are_not_integer() {
    ASSERT(!sample_format_is_integer(SampleFormat::Float32));
    ASSERT(!sample_format_is_integer(SampleFormat::Float64));
}

static void int_formats_are_signed() {
    ASSERT(sample_format_is_signed(SampleFormat::Int16));
    ASSERT(sample_format_is_signed(SampleFormat::Int24));
    ASSERT(sample_format_is_signed(SampleFormat::Int32));
}

static void alignment_matches_container_bytes() {
    ASSERT(sample_format_alignment(SampleFormat::Int16) == 2);
    ASSERT(sample_format_alignment(SampleFormat::Int24) == 4);
    ASSERT(sample_format_alignment(SampleFormat::Int32) == 4);
    ASSERT(sample_format_alignment(SampleFormat::Float32) == 4);
    ASSERT(sample_format_alignment(SampleFormat::Float64) == 8);
}

void run_sample_format_contract_tests(ContractTestRunner& runner) {
    std::cout << "--- SampleFormat ---\n";
    runner.run("sample_format_values_are_distinct", sample_format_values_are_distinct);
    runner.run("valid_bits_of_unknown_is_zero", valid_bits_of_unknown_is_zero);
    runner.run("valid_bits_of_int16_is_16", valid_bits_of_int16_is_16);
    runner.run("valid_bits_of_int24_is_24", valid_bits_of_int24_is_24);
    runner.run("container_bits_of_int24_is_32", container_bits_of_int24_is_32);
    runner.run("int_formats_are_integer", int_formats_are_integer);
    runner.run("float_formats_are_not_integer", float_formats_are_not_integer);
    runner.run("int_formats_are_signed", int_formats_are_signed);
    runner.run("alignment_matches_container_bytes", alignment_matches_container_bytes);
    std::cout << "\n";
}
