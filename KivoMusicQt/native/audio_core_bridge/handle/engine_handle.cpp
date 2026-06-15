// =============================================================================
// Kivo Music Qt - engine_handle.cpp
// AudioCoreBridge: Engine handle implementation
// =============================================================================

#include "engine_handle.hpp"

namespace kivo::qt::audio_bridge::handle {

error::BridgeResult<EngineHandle> EngineHandle::create(
    const loader::AudioCoreFunctions& functions) {

    if (!functions.create || !functions.destroy) {
        return error::BridgeResult<EngineHandle>::failed(
            error::BridgeError::engineCreateFailed(-1));
    }

    kivo_audio_create_info_v1 createInfo{};
    createInfo.struct_size = sizeof(kivo_audio_create_info_v1);
    createInfo.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;

    kivo_audio_handle handle = KIVO_AUDIO_INVALID_HANDLE;
    kivo_audio_result result = functions.create(&createInfo, &handle);

    if (result != KIVO_AUDIO_RESULT_OK
        || handle == KIVO_AUDIO_INVALID_HANDLE) {
        return error::BridgeResult<EngineHandle>::failed(
            error::BridgeError::engineCreateFailed(static_cast<int>(result)));
    }

    return error::BridgeResult<EngineHandle>::ok(
        EngineHandle(handle, functions));
}

EngineHandle::EngineHandle(kivo_audio_handle handle,
                           const loader::AudioCoreFunctions& functions)
    : handle_(handle), functions_(&functions) {}

EngineHandle::~EngineHandle() {
    if (handle_ != KIVO_AUDIO_INVALID_HANDLE && functions_ && functions_->destroy) {
        functions_->destroy(handle_);
        handle_ = KIVO_AUDIO_INVALID_HANDLE;
    }
}

EngineHandle::EngineHandle(EngineHandle&& other) noexcept
    : handle_(other.handle_), functions_(other.functions_) {
    other.handle_ = KIVO_AUDIO_INVALID_HANDLE;
    other.functions_ = nullptr;
}

EngineHandle& EngineHandle::operator=(EngineHandle&& other) noexcept {
    if (this != &other) {
        // Destroy current handle
        if (handle_ != KIVO_AUDIO_INVALID_HANDLE && functions_ && functions_->destroy) {
            functions_->destroy(handle_);
        }

        // Move from other
        handle_ = other.handle_;
        functions_ = other.functions_;

        other.handle_ = KIVO_AUDIO_INVALID_HANDLE;
        other.functions_ = nullptr;
    }
    return *this;
}

} // namespace kivo::qt::audio_bridge::handle
