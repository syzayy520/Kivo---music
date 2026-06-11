#include "../../contract_tests_main.h"
#include "kivo/core/contract/buffer/identity/buffer_id.hpp"
#include "kivo/core/contract/buffer/identity/buffer_generation.hpp"

namespace {

void buffer_id_default_is_zero() {
    kivo::core::contract::BufferId id;
    ASSERT(id.value == 0);
}

void buffer_id_equality() {
    kivo::core::contract::BufferId a{42};
    kivo::core::contract::BufferId b{42};
    ASSERT(a == b);
}

void buffer_id_inequality() {
    kivo::core::contract::BufferId a{1};
    kivo::core::contract::BufferId b{2};
    ASSERT(!(a == b));
}

void buffer_generation_default_is_initial() {
    kivo::core::contract::BufferGeneration gen;
    ASSERT(gen.value() == kivo::core::contract::GenerationId::initial().value());
}

void buffer_generation_equality() {
    kivo::core::contract::BufferGeneration a;
    kivo::core::contract::BufferGeneration b;
    ASSERT(a == b);
}

void buffer_generation_inequality() {
    kivo::core::contract::BufferGeneration a;
    kivo::core::contract::BufferGeneration b{kivo::core::contract::GenerationId::next(a.id)};
    ASSERT(!(a == b));
}

void buffer_generation_value_accessor() {
    kivo::core::contract::BufferGeneration gen;
    ASSERT(gen.value() == gen.id.value());
}

} // namespace

void run_buffer_identity_tests(ContractTestRunner& runner) {
    runner.run("buffer_id_default_is_zero", buffer_id_default_is_zero);
    runner.run("buffer_id_equality", buffer_id_equality);
    runner.run("buffer_id_inequality", buffer_id_inequality);
    runner.run("buffer_generation_default_is_initial", buffer_generation_default_is_initial);
    runner.run("buffer_generation_equality", buffer_generation_equality);
    runner.run("buffer_generation_inequality", buffer_generation_inequality);
    runner.run("buffer_generation_value_accessor", buffer_generation_value_accessor);
}