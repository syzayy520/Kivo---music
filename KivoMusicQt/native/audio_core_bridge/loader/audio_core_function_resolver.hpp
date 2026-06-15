// =============================================================================
// Kivo Music Qt - audio_core_function_resolver.hpp
// AudioCoreBridge: Resolve all Host ABI functions from DLL
// =============================================================================

#pragma once

#include <QString>
#include "kivo/host/abi/api/kivo_audio_api.h"
#include "../error/bridge_error.hpp"

namespace kivo::qt::audio_bridge::loader {

class AudioCoreLibraryLoader;

// Function pointer types for all Host ABI functions
struct AudioCoreFunctions {
    using CreateFn = kivo_audio_result (*)(
        const kivo_audio_create_info_v1* createInfo,
        kivo_audio_handle* outHandle);
    using DestroyFn = kivo_audio_result (*)(kivo_audio_handle handle);
    using GetCapabilitiesFn = kivo_audio_result (*)(
        kivo_audio_handle handle,
        kivo_audio_capabilities_v1* capabilities);
    using OpenSourceFn = kivo_audio_result (*)(
        kivo_audio_handle handle,
        uint64_t commandId,
        uint64_t sessionGeneration,
        const kivo_audio_source_v1* source,
        const kivo_audio_open_configuration_v1* configuration);
    using SubmitCommandFn = kivo_audio_result (*)(
        kivo_audio_handle handle,
        const kivo_audio_command_v1* command);
    using PumpFn = kivo_audio_result (*)(
        kivo_audio_handle handle,
        const kivo_audio_pump_request_v1* request,
        kivo_audio_pump_result_v1* result);
    using GetSnapshotFn = kivo_audio_result (*)(
        kivo_audio_handle handle,
        kivo_audio_snapshot_v1* snapshot);

    CreateFn create = nullptr;
    DestroyFn destroy = nullptr;
    GetCapabilitiesFn getCapabilities = nullptr;
    OpenSourceFn openSource = nullptr;
    SubmitCommandFn submitCommand = nullptr;
    PumpFn pump = nullptr;
    GetSnapshotFn getSnapshot = nullptr;
};

// Resolves all Host ABI functions from a loaded DLL
class AudioCoreFunctionResolver final {
public:
    explicit AudioCoreFunctionResolver(AudioCoreLibraryLoader& loader);

    // Resolve all required functions
    // Returns error if any critical function is missing
    error::VoidResult resolveAll();

    // Check if all functions are resolved
    bool isFullyResolved() const;

    // Get the function table
    const AudioCoreFunctions& functions() const;

private:
    error::VoidResult resolveFunction(void** target, const char* name, bool required);

    AudioCoreLibraryLoader& loader_;
    AudioCoreFunctions functions_;
    bool fullyResolved_ = false;
};

} // namespace kivo::qt::audio_bridge::loader
