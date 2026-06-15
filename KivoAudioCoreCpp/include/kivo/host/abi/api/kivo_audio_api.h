#ifndef KIVO_HOST_ABI_API_KIVO_AUDIO_API_H
#define KIVO_HOST_ABI_API_KIVO_AUDIO_API_H

#include "kivo/host/abi/capability/kivo_audio_capabilities.h"
#include "kivo/host/abi/command/kivo_audio_command.h"
#include "kivo/host/abi/configuration/kivo_audio_configuration.h"
#include "kivo/host/abi/handle/kivo_audio_handle.h"
#include "kivo/host/abi/result/kivo_audio_result.h"
#include "kivo/host/abi/snapshot/kivo_audio_snapshot.h"
#include "kivo/host/abi/source/kivo_audio_source.h"

#if defined(_WIN32)
#define KIVO_AUDIO_CALL __cdecl
#if defined(KIVO_AUDIO_HOST_ABI_BUILD)
#define KIVO_AUDIO_API __declspec(dllexport)
#else
#define KIVO_AUDIO_API __declspec(dllimport)
#endif
#else
#define KIVO_AUDIO_CALL
#define KIVO_AUDIO_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

KIVO_AUDIO_API kivo_audio_result KIVO_AUDIO_CALL kivo_audio_create(
    const kivo_audio_create_info_v1* create_info,
    kivo_audio_handle* out_handle);

KIVO_AUDIO_API kivo_audio_result KIVO_AUDIO_CALL kivo_audio_destroy(
    kivo_audio_handle handle);

KIVO_AUDIO_API kivo_audio_result KIVO_AUDIO_CALL kivo_audio_get_capabilities(
    kivo_audio_handle handle,
    kivo_audio_capabilities_v1* capabilities);

KIVO_AUDIO_API kivo_audio_result KIVO_AUDIO_CALL kivo_audio_open_source(
    kivo_audio_handle handle,
    uint64_t command_id,
    uint64_t session_generation,
    const kivo_audio_source_v1* source,
    const kivo_audio_open_configuration_v1* configuration);

KIVO_AUDIO_API kivo_audio_result KIVO_AUDIO_CALL kivo_audio_submit_command(
    kivo_audio_handle handle,
    const kivo_audio_command_v1* command);

KIVO_AUDIO_API kivo_audio_result KIVO_AUDIO_CALL kivo_audio_pump(
    kivo_audio_handle handle,
    const kivo_audio_pump_request_v1* request,
    kivo_audio_pump_result_v1* result);

KIVO_AUDIO_API kivo_audio_result KIVO_AUDIO_CALL kivo_audio_get_snapshot(
    kivo_audio_handle handle,
    kivo_audio_snapshot_v1* snapshot);

#ifdef __cplusplus
}
#endif

#endif
