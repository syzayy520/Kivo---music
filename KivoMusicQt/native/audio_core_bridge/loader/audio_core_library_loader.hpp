// =============================================================================
// Kivo Music Qt - audio_core_library_loader.hpp
// AudioCoreBridge: DLL loader for kivo_audio_core.dll
// =============================================================================

#pragma once

#include <QString>
#include <memory>
#include "../error/bridge_error.hpp"

namespace kivo::qt::audio_bridge::loader {

// Forward declarations for platform-specific types
struct LoaderImpl;

// Loads and unloads kivo_audio_core.dll
// RAII: automatically unloads on destruction
class AudioCoreLibraryLoader final {
public:
    AudioCoreLibraryLoader();
    ~AudioCoreLibraryLoader();

    // Non-copyable, movable
    AudioCoreLibraryLoader(const AudioCoreLibraryLoader&) = delete;
    AudioCoreLibraryLoader& operator=(const AudioCoreLibraryLoader&) = delete;
    AudioCoreLibraryLoader(AudioCoreLibraryLoader&&) noexcept;
    AudioCoreLibraryLoader& operator=(AudioCoreLibraryLoader&&) noexcept;

    // Load the DLL from the specified path
    // Returns error if DLL not found or incompatible
    error::VoidResult load(const QString& dllPath);

    // Manually unload (automatically called by destructor)
    void unload();

    // Check if DLL is currently loaded
    bool isLoaded() const;

    // Get the loaded DLL path
    QString loadedPath() const;

    // Resolve a function from the loaded DLL
    // Returns nullptr if function not found
    void* resolveFunction(const char* functionName) const;

private:
    std::unique_ptr<LoaderImpl> impl_;
};

} // namespace kivo::qt::audio_bridge::loader
