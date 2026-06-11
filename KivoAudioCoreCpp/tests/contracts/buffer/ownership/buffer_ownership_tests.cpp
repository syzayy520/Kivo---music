#include "../../contract_tests_main.h"
#include "kivo/core/contract/buffer/ownership/buffer_ownership_mode.hpp"
#include "kivo/core/contract/buffer/ownership/buffer_ownership_transfer.hpp"

namespace {

void buffer_ownership_mode_default_is_not_specified() {
    kivo::core::contract::BufferOwnershipMode mode{};
    ASSERT(mode == kivo::core::contract::BufferOwnershipMode::NotSpecified);
}

void buffer_ownership_mode_owned() {
    auto mode = kivo::core::contract::BufferOwnershipMode::Owned;
    ASSERT(mode != kivo::core::contract::BufferOwnershipMode::NotSpecified);
}

void buffer_ownership_mode_borrowed() {
    auto mode = kivo::core::contract::BufferOwnershipMode::Borrowed;
    ASSERT(mode != kivo::core::contract::BufferOwnershipMode::Owned);
}

void buffer_ownership_mode_shared() {
    auto mode = kivo::core::contract::BufferOwnershipMode::Shared;
    ASSERT(mode != kivo::core::contract::BufferOwnershipMode::Borrowed);
}

void buffer_ownership_transfer_default_is_not_specified() {
    kivo::core::contract::BufferOwnershipTransfer transfer{};
    ASSERT(transfer == kivo::core::contract::BufferOwnershipTransfer::NotSpecified);
}

void buffer_ownership_transfer_move() {
    auto transfer = kivo::core::contract::BufferOwnershipTransfer::Move;
    ASSERT(transfer != kivo::core::contract::BufferOwnershipTransfer::NotSpecified);
}

void buffer_ownership_transfer_copy() {
    auto transfer = kivo::core::contract::BufferOwnershipTransfer::Copy;
    ASSERT(transfer != kivo::core::contract::BufferOwnershipTransfer::Move);
}

void buffer_ownership_transfer_share() {
    auto transfer = kivo::core::contract::BufferOwnershipTransfer::Share;
    ASSERT(transfer != kivo::core::contract::BufferOwnershipTransfer::Copy);
}

void buffer_ownership_transfer_delegate() {
    auto transfer = kivo::core::contract::BufferOwnershipTransfer::Delegate;
    ASSERT(transfer != kivo::core::contract::BufferOwnershipTransfer::Share);
}

void buffer_ownership_mode_equality() {
    auto a = kivo::core::contract::BufferOwnershipMode::Owned;
    auto b = kivo::core::contract::BufferOwnershipMode::Owned;
    ASSERT(a == b);
}

void buffer_ownership_transfer_equality() {
    auto a = kivo::core::contract::BufferOwnershipTransfer::Move;
    auto b = kivo::core::contract::BufferOwnershipTransfer::Move;
    ASSERT(a == b);
}

} // namespace

void run_buffer_ownership_tests(ContractTestRunner& runner) {
    runner.run("buffer_ownership_mode_default_is_not_specified", buffer_ownership_mode_default_is_not_specified);
    runner.run("buffer_ownership_mode_owned", buffer_ownership_mode_owned);
    runner.run("buffer_ownership_mode_borrowed", buffer_ownership_mode_borrowed);
    runner.run("buffer_ownership_mode_shared", buffer_ownership_mode_shared);
    runner.run("buffer_ownership_transfer_default_is_not_specified", buffer_ownership_transfer_default_is_not_specified);
    runner.run("buffer_ownership_transfer_move", buffer_ownership_transfer_move);
    runner.run("buffer_ownership_transfer_copy", buffer_ownership_transfer_copy);
    runner.run("buffer_ownership_transfer_share", buffer_ownership_transfer_share);
    runner.run("buffer_ownership_transfer_delegate", buffer_ownership_transfer_delegate);
    runner.run("buffer_ownership_mode_equality", buffer_ownership_mode_equality);
    runner.run("buffer_ownership_transfer_equality", buffer_ownership_transfer_equality);
}