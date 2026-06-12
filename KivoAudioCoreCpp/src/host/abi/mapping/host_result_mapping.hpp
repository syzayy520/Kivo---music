#pragma once

#include "kivo/core/contract/state/phase/core_state.hpp"
#include "kivo/core/playback/runtime/result/playback_runtime_result.hpp"
#include "kivo/host/abi/result/kivo_audio_result.h"
#include "kivo/host/abi/snapshot/kivo_audio_snapshot.h"

namespace kivo::host::abi {

[[nodiscard]] kivo_audio_result map_runtime_result(
    const core::playback::PlaybackRuntimeResult& result) noexcept;

[[nodiscard]] kivo_audio_state map_core_state(
    core::contract::CoreState state) noexcept;

} // namespace kivo::host::abi
