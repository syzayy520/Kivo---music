// =============================================================================
// KivoAudioCoreCpp — output_path_truth_tests.cpp
// Tests for OutputPathTruth contract type
// =============================================================================

#include "kivo/core/contract/output/path/output_path_truth.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_path_is_not_clean() {
    OutputPathTruth path;
    ASSERT(!path.is_path_clean());
}

static void clean_path() {
    OutputPathTruth path;
    path.format_match.formats_equal = true;
    path.transparency.sample_path_transparent = true;
    path.engine.engine_in_path = false;
    ASSERT(path.is_path_clean());
}

static void dirty_path_engine() {
    OutputPathTruth path;
    path.format_match.formats_equal = true;
    path.transparency.sample_path_transparent = true;
    path.engine.engine_in_path = true;
    ASSERT(!path.is_path_clean());
}

static void equality_same() {
    OutputPathTruth a;
    a.format_match.formats_equal = true;
    OutputPathTruth b = a;
    ASSERT(a == b);
}

static void equality_different() {
    OutputPathTruth a;
    a.format_match.formats_equal = true;
    OutputPathTruth b;
    ASSERT(!(a == b));
}

void run_output_path_truth_tests(ContractTestRunner& runner) {
    std::cout << "--- OutputPathTruth ---\n";
    runner.run("default_path_is_not_clean", default_path_is_not_clean);
    runner.run("clean_path", clean_path);
    runner.run("dirty_path_engine", dirty_path_engine);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
