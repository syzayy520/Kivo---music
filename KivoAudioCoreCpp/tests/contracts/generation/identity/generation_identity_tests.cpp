#include "../../contract_tests_main.h"
#include "kivo/core/contract/generation/identity/stream_generation.hpp"
#include "kivo/core/contract/generation/identity/seek_generation.hpp"
#include "kivo/core/contract/generation/identity/flush_generation.hpp"
#include "kivo/core/contract/generation/identity/device_generation.hpp"

namespace {

void stream_generation_default_is_initial() {
    kivo::core::contract::StreamGeneration gen;
    ASSERT(gen.value() == kivo::core::contract::GenerationId::initial().value());
}

void seek_generation_id_default_is_initial() {
    kivo::core::contract::SeekGenerationId gen;
    ASSERT(gen.value() == kivo::core::contract::GenerationId::initial().value());
}

void flush_generation_default_is_initial() {
    kivo::core::contract::FlushGeneration gen;
    ASSERT(gen.value() == kivo::core::contract::GenerationId::initial().value());
}

void device_generation_default_is_initial() {
    kivo::core::contract::DeviceGeneration gen;
    ASSERT(gen.value() == kivo::core::contract::GenerationId::initial().value());
}

void stream_generation_equality() {
    kivo::core::contract::StreamGeneration a;
    kivo::core::contract::StreamGeneration b;
    ASSERT(a == b);
}

void seek_generation_id_inequality() {
    kivo::core::contract::SeekGenerationId a;
    kivo::core::contract::SeekGenerationId b{kivo::core::contract::GenerationId::next(a.id)};
    ASSERT(!(a == b));
}

void flush_generation_copy() {
    kivo::core::contract::FlushGeneration a;
    kivo::core::contract::FlushGeneration b = a;
    ASSERT(a == b);
    ASSERT(a.value() == b.value());
}

void device_generation_value_accessor() {
    kivo::core::contract::DeviceGeneration gen;
    ASSERT(gen.value() == gen.id.value());
}

} // namespace

void run_generation_identity_tests(ContractTestRunner& runner) {
    runner.run("stream_generation_default_is_initial", stream_generation_default_is_initial);
    runner.run("seek_generation_id_default_is_initial", seek_generation_id_default_is_initial);
    runner.run("flush_generation_default_is_initial", flush_generation_default_is_initial);
    runner.run("device_generation_default_is_initial", device_generation_default_is_initial);
    runner.run("stream_generation_equality", stream_generation_equality);
    runner.run("seek_generation_id_inequality", seek_generation_id_inequality);
    runner.run("flush_generation_copy", flush_generation_copy);
    runner.run("device_generation_value_accessor", device_generation_value_accessor);
}