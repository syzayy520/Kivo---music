#include "observability_test_cases.hpp"

#include <iostream>

int main() {
    observability_test::decode_failures_have_stable_categories();
    observability_test::diagnostic_snapshot_rejects_impossible_totals();
    std::cout << "observability tests passed\n";
    return 0;
}
