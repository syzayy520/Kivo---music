// =============================================================================
// Kivo Music Qt - audio_core_library_guard.cpp
// AudioCoreBridge: RAII guard implementation
// =============================================================================

#include "audio_core_library_guard.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

namespace kivo::qt::audio_bridge::loader {

AudioCoreLibraryGuard::AudioCoreLibraryGuard()
    : loader_(std::make_unique<AudioCoreLibraryLoader>()) {}

AudioCoreLibraryGuard::~AudioCoreLibraryGuard() {
    // Automatic cleanup via unique_ptr
}

error::VoidResult AudioCoreLibraryGuard::initialize() {
    QString dllPath = findDefaultDllPath();
    if (dllPath.isEmpty()) {
        return error::failure(
            error::BridgeError::dllLoadFailed(
                "kivo_audio_core.dll not found in default locations"));
    }
    return initialize(dllPath);
}

error::VoidResult AudioCoreLibraryGuard::initialize(const QString& dllPath) {
    ready_ = false;
    return loadAndResolve(dllPath);
}

bool AudioCoreLibraryGuard::isReady() const {
    return ready_;
}

const AudioCoreFunctions& AudioCoreLibraryGuard::functions() const {
    return resolver_->functions();
}

QString AudioCoreLibraryGuard::loadedPath() const {
    return loader_ ? loader_->loadedPath() : QString();
}

error::VoidResult AudioCoreLibraryGuard::loadAndResolve(const QString& dllPath) {
    // Step 1: Load DLL
    auto loadResult = loader_->load(dllPath);
    if (loadResult.isFailed()) {
        return loadResult;
    }

    // Step 2: Resolve functions
    resolver_ = std::make_unique<AudioCoreFunctionResolver>(*loader_);
    auto resolveResult = resolver_->resolveAll();
    if (resolveResult.isFailed()) {
        loader_->unload();
        return resolveResult;
    }

    ready_ = true;
    return error::success();
}

QString AudioCoreLibraryGuard::findDefaultDllPath() {
    QStringList candidates;

    // 1. Same directory as executable
    QString exeDir = QCoreApplication::applicationDirPath();
    candidates << QDir(exeDir).filePath("kivo_audio_core.dll");

    // 2. One level up (for development builds)
    candidates << QDir(exeDir).filePath("../kivo_audio_core.dll");

    // 3. KivoAudioCoreCpp build directories (development)
    QString projectRoot = QDir(exeDir).filePath("../../");
    candidates << QDir(projectRoot).filePath(
        "KivoAudioCoreCpp/build/Release/kivo_audio_core.dll");
    candidates << QDir(projectRoot).filePath(
        "KivoAudioCoreCpp/out/p0-p-precommit-release/Release/kivo_audio_core.dll");

    // 4. Check each candidate
    for (const QString& path : candidates) {
        QFileInfo info(path);
        if (info.exists() && info.isFile()) {
            return QDir::toNativeSeparators(info.absoluteFilePath());
        }
    }

    return QString();
}

} // namespace kivo::qt::audio_bridge::loader
