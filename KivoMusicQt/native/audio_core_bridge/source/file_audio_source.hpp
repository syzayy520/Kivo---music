// =============================================================================
// Kivo Music Qt - file_audio_source.hpp
// AudioCoreBridge: Host ABI file source callback lease
// =============================================================================

#pragma once

#include "../error/bridge_error.hpp"
#include "../loader/audio_core_function_resolver.hpp"
#include <QString>

namespace kivo::qt::audio_bridge::source {

class FileAudioSourceLease final {
public:
    static error::BridgeResult<FileAudioSourceLease> open(
        const QString& filePath,
        uint64_t sourceIdentity,
        uint64_t sourceGeneration);

    ~FileAudioSourceLease();

    FileAudioSourceLease(const FileAudioSourceLease&) = delete;
    FileAudioSourceLease& operator=(const FileAudioSourceLease&) = delete;
    FileAudioSourceLease(FileAudioSourceLease&& other) noexcept;
    FileAudioSourceLease& operator=(FileAudioSourceLease&& other) noexcept;

    [[nodiscard]] const kivo_audio_source_v1& abiSource() const noexcept;
    void releaseToHost() noexcept;

private:
    explicit FileAudioSourceLease(kivo_audio_source_v1 source) noexcept;
    void releaseOwnedContext() noexcept;

    kivo_audio_source_v1 source_{};
    bool ownsContext_ = true;
};

} // namespace kivo::qt::audio_bridge::source
