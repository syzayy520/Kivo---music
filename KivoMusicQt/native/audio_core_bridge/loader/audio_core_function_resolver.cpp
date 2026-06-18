// =============================================================================
// Kivo Music Qt - audio_core_function_resolver.cpp
// AudioCoreBridge: Function resolution implementation
// =============================================================================

#include "audio_core_function_resolver.hpp"
#include "audio_core_library_loader.hpp"

namespace kivo::qt::audio_bridge::loader {

AudioCoreFunctionResolver::AudioCoreFunctionResolver(AudioCoreLibraryLoader& loader)
    : loader_(loader) {}

error::VoidResult AudioCoreFunctionResolver::resolveAll() {
    if (!loader_.isLoaded()) {
        return error::failure(
            error::BridgeError::functionResolveFailed("DLL not loaded"));
    }

    auto result = resolveFunction(
        reinterpret_cast<void**>(&functions_.create),
        "kivo_audio_create", true);
    if (result.isFailed()) return result;

    result = resolveFunction(
        reinterpret_cast<void**>(&functions_.destroy),
        "kivo_audio_destroy", true);
    if (result.isFailed()) return result;

    result = resolveFunction(
        reinterpret_cast<void**>(&functions_.getCapabilities),
        "kivo_audio_get_capabilities", true);
    if (result.isFailed()) return result;

    result = resolveFunction(
        reinterpret_cast<void**>(&functions_.openSource),
        "kivo_audio_open_source", true);
    if (result.isFailed()) return result;

    result = resolveFunction(
        reinterpret_cast<void**>(&functions_.submitCommand),
        "kivo_audio_submit_command", true);
    if (result.isFailed()) return result;

    result = resolveFunction(
        reinterpret_cast<void**>(&functions_.pump),
        "kivo_audio_pump", true);
    if (result.isFailed()) return result;

    result = resolveFunction(
        reinterpret_cast<void**>(&functions_.getSnapshot),
        "kivo_audio_get_snapshot", true);
    if (result.isFailed()) return result;

    // Optional (ABI 1.2.0). A 1.1.0 core leaves this null and the truth surface
    // degrades to "Unknown" — not a load failure.
    (void)resolveFunction(
        reinterpret_cast<void**>(&functions_.getTruth),
        "kivo_audio_get_truth", false);

    fullyResolved_ = true;
    return error::success();
}

bool AudioCoreFunctionResolver::isFullyResolved() const {
    return fullyResolved_;
}

const AudioCoreFunctions& AudioCoreFunctionResolver::functions() const {
    return functions_;
}

error::VoidResult AudioCoreFunctionResolver::resolveFunction(
    void** target, const char* name, bool required) {

    *target = loader_.resolveFunction(name);

    if (*target == nullptr && required) {
        return error::failure(
            error::BridgeError::functionResolveFailed(QString::fromUtf8(name)));
    }

    return error::success();
}

} // namespace kivo::qt::audio_bridge::loader
