#include "kivo/host/abi/api/kivo_audio_api.h"

uint64_t kivo_host_abi_c_contract_size(void) {
    return (uint64_t)sizeof(kivo_audio_create_info_v1)
        + (uint64_t)sizeof(kivo_audio_source_v1)
        + (uint64_t)sizeof(kivo_audio_open_configuration_v1)
        + (uint64_t)sizeof(kivo_audio_snapshot_v1);
}
