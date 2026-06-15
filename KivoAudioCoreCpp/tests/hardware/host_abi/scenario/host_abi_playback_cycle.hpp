#pragma once

#include <filesystem>

namespace kivo::hardware_test::host_abi {

[[nodiscard]] int run_host_abi_playback(
    const std::filesystem::path& path);

} // namespace kivo::hardware_test::host_abi
