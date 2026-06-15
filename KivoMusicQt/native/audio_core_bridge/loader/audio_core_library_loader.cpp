// =============================================================================
// Kivo Music Qt - audio_core_library_loader.cpp
// AudioCoreBridge: DLL loader implementation (Windows)
// =============================================================================

#include "audio_core_library_loader.hpp"
#include <windows.h>

namespace kivo::qt::audio_bridge::loader {

// Platform-specific implementation
struct LoaderImpl {
    HMODULE dllHandle = nullptr;
    QString loadedPath;
};

AudioCoreLibraryLoader::AudioCoreLibraryLoader()
    : impl_(std::make_unique<LoaderImpl>()) {}

AudioCoreLibraryLoader::~AudioCoreLibraryLoader() {
    unload();
}

AudioCoreLibraryLoader::AudioCoreLibraryLoader(AudioCoreLibraryLoader&&) noexcept = default;
AudioCoreLibraryLoader& AudioCoreLibraryLoader::operator=(AudioCoreLibraryLoader&&) noexcept = default;

error::VoidResult AudioCoreLibraryLoader::load(const QString& dllPath) {
    if (isLoaded()) {
        unload();
    }

    // Convert QString to wide string for Windows API
    std::wstring widePath = dllPath.toStdWString();

    // Load DLL with explicit path
    impl_->dllHandle = ::LoadLibraryW(widePath.c_str());

    if (impl_->dllHandle == nullptr) {
        DWORD errorCode = ::GetLastError();
        return error::failure(
            error::BridgeError::dllLoadFailed(
                QString("%1 (Win32 error: %2)").arg(dllPath).arg(errorCode)));
    }

    impl_->loadedPath = dllPath;
    return error::success();
}

void AudioCoreLibraryLoader::unload() {
    if (impl_->dllHandle != nullptr) {
        ::FreeLibrary(impl_->dllHandle);
        impl_->dllHandle = nullptr;
        impl_->loadedPath.clear();
    }
}

bool AudioCoreLibraryLoader::isLoaded() const {
    return impl_->dllHandle != nullptr;
}

QString AudioCoreLibraryLoader::loadedPath() const {
    return impl_->loadedPath;
}

void* AudioCoreLibraryLoader::resolveFunction(const char* functionName) const {
    if (!isLoaded()) {
        return nullptr;
    }

    return reinterpret_cast<void*>(
        ::GetProcAddress(impl_->dllHandle, functionName));
}

} // namespace kivo::qt::audio_bridge::loader
