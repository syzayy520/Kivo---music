#include "../../contract_tests_main.h"
#include "kivo/core/contract/queue/boundary/queue_boundary_mode.hpp"

namespace {

void queue_boundary_mode_default_is_not_specified() {
    kivo::core::contract::QueueBoundaryMode mode{};
    ASSERT(mode == kivo::core::contract::QueueBoundaryMode::NotSpecified);
}

void queue_boundary_mode_spsc() {
    auto mode = kivo::core::contract::QueueBoundaryMode::SPSC;
    ASSERT(mode != kivo::core::contract::QueueBoundaryMode::NotSpecified);
}

void queue_boundary_mode_mpsc() {
    auto mode = kivo::core::contract::QueueBoundaryMode::MPSC;
    ASSERT(mode != kivo::core::contract::QueueBoundaryMode::SPSC);
}

void queue_boundary_mode_spmc() {
    auto mode = kivo::core::contract::QueueBoundaryMode::SPMC;
    ASSERT(mode != kivo::core::contract::QueueBoundaryMode::MPSC);
}

void queue_boundary_mode_mpmc() {
    auto mode = kivo::core::contract::QueueBoundaryMode::MPMC;
    ASSERT(mode != kivo::core::contract::QueueBoundaryMode::SPMC);
}

void queue_boundary_mode_equality() {
    auto a = kivo::core::contract::QueueBoundaryMode::SPSC;
    auto b = kivo::core::contract::QueueBoundaryMode::SPSC;
    ASSERT(a == b);
}

void queue_boundary_mode_inequality() {
    auto a = kivo::core::contract::QueueBoundaryMode::SPSC;
    auto b = kivo::core::contract::QueueBoundaryMode::MPSC;
    ASSERT(a != b);
}

} // namespace

void run_queue_boundary_tests(ContractTestRunner& runner) {
    runner.run("queue_boundary_mode_default_is_not_specified", queue_boundary_mode_default_is_not_specified);
    runner.run("queue_boundary_mode_spsc", queue_boundary_mode_spsc);
    runner.run("queue_boundary_mode_mpsc", queue_boundary_mode_mpsc);
    runner.run("queue_boundary_mode_spmc", queue_boundary_mode_spmc);
    runner.run("queue_boundary_mode_mpmc", queue_boundary_mode_mpmc);
    runner.run("queue_boundary_mode_equality", queue_boundary_mode_equality);
    runner.run("queue_boundary_mode_inequality", queue_boundary_mode_inequality);
}