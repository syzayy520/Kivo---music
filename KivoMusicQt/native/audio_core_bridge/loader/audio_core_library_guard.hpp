// =============================================================================
// Kivo Music Qt - audio_core_library_guard.hpp
// AudioCoreBridge: RAII guard for library lifecycle
// =============================================================================

#pragma once

#include "audio_core_library_loader.hpp"
#include "audio_core_function_resolver.hpp"
#include <memory>

namespace kivo::qt::audio_bridge::loader {

// RAII guard that ensures proper initialization and cleanup
// Combines loader + resolver into one easy-to-use interface
class AudioCoreLibraryGuard final {
public:
    AudioCoreLibraryGuard();
    ~AudioCoreLibraryGuard();

    // Non-copyable, non-movable (owns critical resources)
    AudioCoreLibraryGuard(const AudioCoreLibraryGuard&) = delete;
    AudioCoreLibraryGuard& operator=(const AudioCoreLibraryGuard&) = delete;
    AudioCoreLibraryGuard(AudioCoreLibraryGuard&&) = delete;
    AudioCoreLibraryGuard& operator=(AudioCoreLibraryGuard&&) = delete;

    // Initialize: load DLL and resolve functions
    // Tries common locations: ./kivo_audio_core.dll, ../KivoAudioCoreCpp/build/...
    error::VoidResult initialize();

    // Initialize with explicit DLL path
    error::VoidResult initialize(const QString& dllPath);

    // Check if ready to use
    bool isReady() const;

    // Get function table (only valid if isReady())
    const AudioCoreFunctions& functions() const;

    // Get loaded DLL path
    QString loadedPath() const;

private:
    error::VoidResult loadAndResolve(const QString& dllPath);
    static QString findDefaultDllPath();

    std::unique_ptr<AudioCoreLibraryLoader> loader_;
    std::unique_ptr<AudioCoreFunctionResolver> resolver_;
    bool ready_ = false;
};

} // namespace kivo::qt::audio_bridge::loader
