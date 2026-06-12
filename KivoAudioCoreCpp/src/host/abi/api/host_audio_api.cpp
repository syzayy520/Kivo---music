#include "kivo/host/abi/api/kivo_audio_api.h"

#include <memory>
#include <new>

#include "host/abi/engine/host_audio_engine.hpp"
#include "host/abi/handle/host_handle_registry.hpp"
#include "host/abi/source/host_callback_byte_source.hpp"
#include "host/abi/validation/host_abi_validation.hpp"

namespace {

[[nodiscard]] std::shared_ptr<kivo::host::abi::HostAudioEngine>
engine_for(kivo_audio_handle handle) noexcept {
    return kivo::host::abi::HostHandleRegistry::instance().acquire(
        handle);
}

[[nodiscard]] bool valid_source(
    const kivo_audio_source_v1* source) noexcept {
    using kivo::host::abi::valid_input_structure;
    if (!valid_input_structure(source)
        || source->source_identity == 0u
        || source->source_generation == 0u
        || source->read == nullptr
        || source->release == nullptr) {
        return false;
    }
    const uint32_t known_flags =
        KIVO_AUDIO_SOURCE_FLAG_SEEKABLE
        | KIVO_AUDIO_SOURCE_FLAG_SIZE_KNOWN;
    if ((source->flags & ~known_flags) != 0u) {
        return false;
    }
    return (source->flags & KIVO_AUDIO_SOURCE_FLAG_SEEKABLE) == 0u
        || source->seek != nullptr;
}

} // namespace

extern "C" {

kivo_audio_result KIVO_AUDIO_CALL kivo_audio_create(
    const kivo_audio_create_info_v1* create_info,
    kivo_audio_handle* out_handle) {
    using namespace kivo::host::abi;
    if (out_handle == nullptr) {
        return KIVO_AUDIO_RESULT_INVALID_ARGUMENT;
    }
    *out_handle = KIVO_AUDIO_INVALID_HANDLE;
    if (!valid_input_structure(create_info)) {
        return KIVO_AUDIO_RESULT_INVALID_STRUCT;
    }
    try {
        auto engine = std::make_shared<HostAudioEngine>(*create_info);
        const auto handle =
            HostHandleRegistry::instance().insert(std::move(engine));
        if (handle == 0u) {
            return KIVO_AUDIO_RESULT_INTERNAL_FAILURE;
        }
        *out_handle = handle;
        return KIVO_AUDIO_RESULT_OK;
    } catch (const std::bad_alloc&) {
        return KIVO_AUDIO_RESULT_OUT_OF_MEMORY;
    } catch (...) {
        return KIVO_AUDIO_RESULT_INTERNAL_FAILURE;
    }
}

kivo_audio_result KIVO_AUDIO_CALL kivo_audio_destroy(
    kivo_audio_handle handle) {
    auto engine =
        kivo::host::abi::HostHandleRegistry::instance().remove(handle);
    if (!engine) {
        return KIVO_AUDIO_RESULT_INVALID_HANDLE;
    }
    engine->shutdown_for_destroy();
    return KIVO_AUDIO_RESULT_OK;
}

kivo_audio_result KIVO_AUDIO_CALL kivo_audio_get_capabilities(
    kivo_audio_handle handle,
    kivo_audio_capabilities_v1* capabilities) {
    if (!kivo::host::abi::valid_output_structure(capabilities)) {
        return KIVO_AUDIO_RESULT_INVALID_STRUCT;
    }
    auto engine = engine_for(handle);
    return engine
        ? engine->capabilities(*capabilities)
        : KIVO_AUDIO_RESULT_INVALID_HANDLE;
}

kivo_audio_result KIVO_AUDIO_CALL kivo_audio_open_source(
    kivo_audio_handle handle,
    uint64_t command_id,
    uint64_t session_generation,
    const kivo_audio_source_v1* source,
    const kivo_audio_open_configuration_v1* configuration) {
    using namespace kivo::host::abi;
    if (!valid_source(source)
        || !valid_input_structure(configuration)) {
        return KIVO_AUDIO_RESULT_INVALID_STRUCT;
    }
    auto engine = engine_for(handle);
    if (!engine) {
        return KIVO_AUDIO_RESULT_INVALID_HANDLE;
    }
    try {
        auto adapter =
            std::make_unique<HostCallbackByteSource>(*source);
        return engine->open_source(
            command_id,
            session_generation,
            std::move(adapter),
            *configuration);
    } catch (const std::bad_alloc&) {
        try {
            source->release(source->source_context);
        } catch (...) {
        }
        return KIVO_AUDIO_RESULT_OUT_OF_MEMORY;
    } catch (...) {
        try {
            source->release(source->source_context);
        } catch (...) {
        }
        return KIVO_AUDIO_RESULT_INTERNAL_FAILURE;
    }
}

kivo_audio_result KIVO_AUDIO_CALL kivo_audio_submit_command(
    kivo_audio_handle handle,
    const kivo_audio_command_v1* command) {
    if (!kivo::host::abi::valid_input_structure(command)) {
        return KIVO_AUDIO_RESULT_INVALID_STRUCT;
    }
    auto engine = engine_for(handle);
    return engine
        ? engine->submit(*command)
        : KIVO_AUDIO_RESULT_INVALID_HANDLE;
}

kivo_audio_result KIVO_AUDIO_CALL kivo_audio_pump(
    kivo_audio_handle handle,
    const kivo_audio_pump_request_v1* request,
    kivo_audio_pump_result_v1* result) {
    using namespace kivo::host::abi;
    if (!valid_input_structure(request)
        || !valid_output_structure(result)) {
        return KIVO_AUDIO_RESULT_INVALID_STRUCT;
    }
    auto engine = engine_for(handle);
    return engine
        ? engine->pump(*request, *result)
        : KIVO_AUDIO_RESULT_INVALID_HANDLE;
}

kivo_audio_result KIVO_AUDIO_CALL kivo_audio_get_snapshot(
    kivo_audio_handle handle,
    kivo_audio_snapshot_v1* snapshot) {
    if (!kivo::host::abi::valid_output_structure(snapshot)) {
        return KIVO_AUDIO_RESULT_INVALID_STRUCT;
    }
    auto engine = engine_for(handle);
    return engine
        ? engine->snapshot(*snapshot)
        : KIVO_AUDIO_RESULT_INVALID_HANDLE;
}

} // extern "C"
