#include "../../contract_tests_main.h"
#include "kivo/core/contract/buffer/share/buffer_share_semantics.hpp"

namespace {

void buffer_share_semantics_default_is_not_specified() {
    kivo::core::contract::BufferShareSemantics semantics{};
    ASSERT(semantics == kivo::core::contract::BufferShareSemantics::NotSpecified);
}

void buffer_share_semantics_exclusive() {
    auto semantics = kivo::core::contract::BufferShareSemantics::Exclusive;
    ASSERT(semantics != kivo::core::contract::BufferShareSemantics::NotSpecified);
}

void buffer_share_semantics_shared_read_only() {
    auto semantics = kivo::core::contract::BufferShareSemantics::SharedReadOnly;
    ASSERT(semantics != kivo::core::contract::BufferShareSemantics::Exclusive);
}

void buffer_share_semantics_shared_read_write() {
    auto semantics = kivo::core::contract::BufferShareSemantics::SharedReadWrite;
    ASSERT(semantics != kivo::core::contract::BufferShareSemantics::SharedReadOnly);
}

void buffer_share_semantics_equality() {
    auto a = kivo::core::contract::BufferShareSemantics::Exclusive;
    auto b = kivo::core::contract::BufferShareSemantics::Exclusive;
    ASSERT(a == b);
}

void buffer_share_semantics_inequality() {
    auto a = kivo::core::contract::BufferShareSemantics::Exclusive;
    auto b = kivo::core::contract::BufferShareSemantics::SharedReadOnly;
    ASSERT(a != b);
}

} // namespace

void run_buffer_share_tests(ContractTestRunner& runner) {
    runner.run("buffer_share_semantics_default_is_not_specified", buffer_share_semantics_default_is_not_specified);
    runner.run("buffer_share_semantics_exclusive", buffer_share_semantics_exclusive);
    runner.run("buffer_share_semantics_shared_read_only", buffer_share_semantics_shared_read_only);
    runner.run("buffer_share_semantics_shared_read_write", buffer_share_semantics_shared_read_write);
    runner.run("buffer_share_semantics_equality", buffer_share_semantics_equality);
    runner.run("buffer_share_semantics_inequality", buffer_share_semantics_inequality);
}