#include "../platform/windows/endpoint/endpoint_inventory.hpp"
#include "../scenario/device_matrix_report.hpp"
#include "../scenario/device_matrix_validation.hpp"
#include "../serialization/device_matrix_json_report.hpp"

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

namespace {

struct Options {
    std::string json_path;
    std::string machine_id{"unassigned"};
};

[[nodiscard]] bool parse_options(
    int argc,
    char** argv,
    Options& options) {
    for (int index = 1; index < argc; ++index) {
        const std::string argument{argv[index]};
        if ((argument == "--json" || argument == "--machine-id")
            && index + 1 >= argc) {
            return false;
        }
        if (argument == "--json") {
            options.json_path = argv[++index];
        } else if (argument == "--machine-id") {
            options.machine_id = argv[++index];
        } else {
            return false;
        }
    }
    return !options.machine_id.empty();
}

} // namespace

int main(int argc, char** argv) {
    try {
        Options options;
        if (!parse_options(argc, argv, options)) {
            std::cerr
                << "Usage: kivo_wasapi_device_matrix "
                   "[--json <path>] [--machine-id <lab-id>]\n";
            return 3;
        }
        const auto inventory =
            device_matrix::windows::enumerate_render_endpoints();
        device_matrix::write_device_matrix_report(std::cout, inventory);

        const auto valid = device_matrix::validate_inventory(inventory);
        if (!options.json_path.empty()) {
            std::ofstream json{
                options.json_path,
                std::ios::binary | std::ios::trunc};
            if (!json) {
                std::cerr << "Cannot open JSON evidence path: "
                          << options.json_path << '\n';
                return 4;
            }
            device_matrix::write_device_matrix_json(
                json,
                inventory,
                options.machine_id,
                valid);
            if (!json) {
                std::cerr << "Cannot write JSON evidence path: "
                          << options.json_path << '\n';
                return 5;
            }
        }
        std::cout << "\nInventory gate: " << (valid ? "PASS" : "FAIL")
                  << '\n';
        return valid ? 0 : 1;
    } catch (const std::exception& exception) {
        std::cerr << "Device inventory failed: " << exception.what() << '\n';
        return 2;
    }
}
