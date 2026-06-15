#include "../scenario/host_abi_playback_cycle.hpp"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: kivo_host_abi_playback_smoke <audio-file>\n";
        return 1;
    }
    return kivo::hardware_test::host_abi::run_host_abi_playback(
        std::filesystem::path{argv[1]});
}
