// =============================================================================
// KivoAudioCoreCpp — sample_transparency_tests.cpp
// Tests for SampleTransparency contract type
// =============================================================================

#include "kivo/core/contract/output/path/sample_transparency.hpp"
#include "../../contract_tests_main.h"

using namespace kivo::core::contract;

static void default_is_not_transparent() {
    SampleTransparency st;
    ASSERT(!st.is_transparent());
}

static void sample_path_transparent() {
    SampleTransparency st;
    st.sample_path_transparent = true;
    st.bit_depth_preserved = true;
    st.encoding_preserved = true;
    ASSERT(st.is_transparent());
}

static void partial_transparency() {
    SampleTransparency st;
    st.sample_path_transparent = true;
    st.bit_depth_preserved = true;
    st.encoding_preserved = false;
    ASSERT(st.is_transparent());
}

static void equality_same() {
    SampleTransparency a;
    a.sample_path_transparent = true;
    SampleTransparency b = a;
    ASSERT(a == b);
}

static void equality_different() {
    SampleTransparency a;
    a.bit_depth_preserved = true;
    SampleTransparency b;
    ASSERT(!(a == b));
}

void run_sample_transparency_tests(ContractTestRunner& runner) {
    std::cout << "--- SampleTransparency ---\n";
    runner.run("default_is_not_transparent", default_is_not_transparent);
    runner.run("sample_path_transparent", sample_path_transparent);
    runner.run("partial_transparency", partial_transparency);
    runner.run("equality_same", equality_same);
    runner.run("equality_different", equality_different);
    std::cout << "\n";
}
