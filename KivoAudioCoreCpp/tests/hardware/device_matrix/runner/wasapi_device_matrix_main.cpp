#include "../platform/windows/endpoint/endpoint_inventory.hpp"
#include "../scenario/device_matrix_report.hpp"
#include "../scenario/device_matrix_validation.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        const auto inventory =
            device_matrix::windows::enumerate_render_endpoints();
        device_matrix::write_device_matrix_report(std::cout, inventory);

        const auto valid = device_matrix::validate_inventory(inventory);
        std::cout << "\nInventory gate: " << (valid ? "PASS" : "FAIL")
                  << '\n';
        return valid ? 0 : 1;
    } catch (const std::exception& exception) {
        std::cerr << "Device inventory failed: " << exception.what() << '\n';
        return 2;
    }
}
