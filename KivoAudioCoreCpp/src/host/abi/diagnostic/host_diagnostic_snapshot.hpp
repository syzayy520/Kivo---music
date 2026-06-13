#pragma once

#include "kivo/core/observability/snapshot/audio_diagnostic_snapshot.hpp"
#include "kivo/core/playback/runtime/snapshot/playback_runtime_snapshot.hpp"
#include "kivo/platform/windows/wasapi/diagnostics/wasapi_diagnostics.hpp"

namespace kivo::host::abi {

[[nodiscard]] core::observability::AudioDiagnosticSnapshot
make_host_diagnostic_snapshot(
    const core::playback::PlaybackRuntimeSnapshot& runtime,
    const platform::windows::wasapi::WasapiDiagnostics& wasapi) noexcept;

} // namespace kivo::host::abi
