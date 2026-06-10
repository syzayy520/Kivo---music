// =============================================================================
// KivoAudioCoreCpp — seek_intent_tests.cpp
// Tests for SeekOrigin/SeekIntent contract types
// =============================================================================

#include "kivo/core/contract/seek/intent/seek_intent.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

// =============================================================================
// Tests
// =============================================================================
static void seek_origin_values() {
    ASSERT(SeekOrigin::Absolute != SeekOrigin::Relative);
    ASSERT(SeekOrigin::Relative != SeekOrigin::RelativeToEnd);
    ASSERT(SeekOrigin::Absolute != SeekOrigin::RelativeToEnd);
}

static void seek_intent_default_construction_is_valid() {
    SeekIntent intent;
    ASSERT(intent.is_valid());
}

static void seek_intent_absolute_valid() {
    SeekIntent intent{1000, SeekOrigin::Absolute, true};
    ASSERT(intent.is_valid());
    ASSERT(intent.offset == 1000);
    ASSERT(intent.origin == SeekOrigin::Absolute);
    ASSERT(intent.request_flush == true);
}

static void seek_intent_absolute_zero_offset_is_valid() {
    SeekIntent intent{0, SeekOrigin::Absolute, true};
    ASSERT(intent.is_valid());
}

static void seek_intent_absolute_negative_offset_is_invalid() {
    SeekIntent intent{-1, SeekOrigin::Absolute, true};
    ASSERT(!intent.is_valid());
}

static void seek_intent_relative_valid() {
    SeekIntent intent{-500, SeekOrigin::Relative, false};
    ASSERT(intent.is_valid());
    ASSERT(intent.offset == -500);
    ASSERT(intent.origin == SeekOrigin::Relative);
    ASSERT(intent.request_flush == false);
}

static void seek_intent_relative_zero_offset_is_valid() {
    SeekIntent intent{0, SeekOrigin::Relative, true};
    ASSERT(intent.is_valid());
}

static void seek_intent_relative_to_end_valid() {
    SeekIntent intent{-1000, SeekOrigin::RelativeToEnd, true};
    ASSERT(intent.is_valid());
}

static void seek_intent_equality_same() {
    SeekIntent a{1000, SeekOrigin::Absolute, true};
    SeekIntent b{1000, SeekOrigin::Absolute, true};
    ASSERT(a == b);
}

static void seek_intent_equality_different_offset() {
    SeekIntent a{1000, SeekOrigin::Absolute, true};
    SeekIntent b{2000, SeekOrigin::Absolute, true};
    ASSERT(!(a == b));
}

static void seek_intent_equality_different_origin() {
    SeekIntent a{1000, SeekOrigin::Absolute, true};
    SeekIntent b{1000, SeekOrigin::Relative, true};
    ASSERT(!(a == b));
}

static void seek_intent_equality_different_flush() {
    SeekIntent a{1000, SeekOrigin::Absolute, true};
    SeekIntent b{1000, SeekOrigin::Absolute, false};
    ASSERT(!(a == b));
}

// =============================================================================
// Runner
// =============================================================================
void run_seek_intent_tests(ContractTestRunner& runner) {
    std::cout << "--- SeekOrigin/SeekIntent ---\n";
    runner.run("seek_origin_values", seek_origin_values);
    runner.run("seek_intent_default_construction_is_valid", seek_intent_default_construction_is_valid);
    runner.run("seek_intent_absolute_valid", seek_intent_absolute_valid);
    runner.run("seek_intent_absolute_zero_offset_is_valid", seek_intent_absolute_zero_offset_is_valid);
    runner.run("seek_intent_absolute_negative_offset_is_invalid", seek_intent_absolute_negative_offset_is_invalid);
    runner.run("seek_intent_relative_valid", seek_intent_relative_valid);
    runner.run("seek_intent_relative_zero_offset_is_valid", seek_intent_relative_zero_offset_is_valid);
    runner.run("seek_intent_relative_to_end_valid", seek_intent_relative_to_end_valid);
    runner.run("seek_intent_equality_same", seek_intent_equality_same);
    runner.run("seek_intent_equality_different_offset", seek_intent_equality_different_offset);
    runner.run("seek_intent_equality_different_origin", seek_intent_equality_different_origin);
    runner.run("seek_intent_equality_different_flush", seek_intent_equality_different_flush);
    std::cout << "\n";
}
