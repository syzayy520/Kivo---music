#include "../../contract_tests_main.h"
#include "kivo/core/contract/buffer/lifetime/buffer_lifetime_proof.hpp"
#include "kivo/core/contract/buffer/lifetime/buffer_reuse_policy.hpp"

namespace {

void buffer_lifetime_proof_default_is_not_specified() {
    kivo::core::contract::BufferLifetimeProof proof{};
    ASSERT(proof == kivo::core::contract::BufferLifetimeProof::NotSpecified);
}

void buffer_lifetime_proof_scoped() {
    auto proof = kivo::core::contract::BufferLifetimeProof::Scoped;
    ASSERT(proof != kivo::core::contract::BufferLifetimeProof::NotSpecified);
}

void buffer_lifetime_proof_reference_counted() {
    auto proof = kivo::core::contract::BufferLifetimeProof::ReferenceCounted;
    ASSERT(proof != kivo::core::contract::BufferLifetimeProof::Scoped);
}

void buffer_lifetime_proof_arena_managed() {
    auto proof = kivo::core::contract::BufferLifetimeProof::ArenaManaged;
    ASSERT(proof != kivo::core::contract::BufferLifetimeProof::ReferenceCounted);
}

void buffer_lifetime_proof_manual_free() {
    auto proof = kivo::core::contract::BufferLifetimeProof::ManualFree;
    ASSERT(proof != kivo::core::contract::BufferLifetimeProof::ArenaManaged);
}

void buffer_reuse_policy_default_is_not_specified() {
    kivo::core::contract::BufferReusePolicy policy{};
    ASSERT(policy == kivo::core::contract::BufferReusePolicy::NotSpecified);
}

void buffer_reuse_policy_single_use() {
    auto policy = kivo::core::contract::BufferReusePolicy::SingleUse;
    ASSERT(policy != kivo::core::contract::BufferReusePolicy::NotSpecified);
}

void buffer_reuse_policy_reusable() {
    auto policy = kivo::core::contract::BufferReusePolicy::Reusable;
    ASSERT(policy != kivo::core::contract::BufferReusePolicy::SingleUse);
}

void buffer_reuse_policy_pool_managed() {
    auto policy = kivo::core::contract::BufferReusePolicy::PoolManaged;
    ASSERT(policy != kivo::core::contract::BufferReusePolicy::Reusable);
}

void buffer_lifetime_proof_equality() {
    auto a = kivo::core::contract::BufferLifetimeProof::Scoped;
    auto b = kivo::core::contract::BufferLifetimeProof::Scoped;
    ASSERT(a == b);
}

void buffer_reuse_policy_equality() {
    auto a = kivo::core::contract::BufferReusePolicy::Reusable;
    auto b = kivo::core::contract::BufferReusePolicy::Reusable;
    ASSERT(a == b);
}

} // namespace

void run_buffer_lifetime_tests(ContractTestRunner& runner) {
    runner.run("buffer_lifetime_proof_default_is_not_specified", buffer_lifetime_proof_default_is_not_specified);
    runner.run("buffer_lifetime_proof_scoped", buffer_lifetime_proof_scoped);
    runner.run("buffer_lifetime_proof_reference_counted", buffer_lifetime_proof_reference_counted);
    runner.run("buffer_lifetime_proof_arena_managed", buffer_lifetime_proof_arena_managed);
    runner.run("buffer_lifetime_proof_manual_free", buffer_lifetime_proof_manual_free);
    runner.run("buffer_reuse_policy_default_is_not_specified", buffer_reuse_policy_default_is_not_specified);
    runner.run("buffer_reuse_policy_single_use", buffer_reuse_policy_single_use);
    runner.run("buffer_reuse_policy_reusable", buffer_reuse_policy_reusable);
    runner.run("buffer_reuse_policy_pool_managed", buffer_reuse_policy_pool_managed);
    runner.run("buffer_lifetime_proof_equality", buffer_lifetime_proof_equality);
    runner.run("buffer_reuse_policy_equality", buffer_reuse_policy_equality);
}