#pragma once

#include <filesystem>

#include "kivo/host/abi/source/kivo_audio_source.h"

namespace kivo::hardware_test::host_abi {

[[nodiscard]] kivo_audio_source_v1 make_local_callback_source(
    const std::filesystem::path& path);

} // namespace kivo::hardware_test::host_abi
