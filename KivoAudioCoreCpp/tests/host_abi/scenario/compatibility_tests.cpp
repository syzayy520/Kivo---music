#include "../fixture/host_abi_test_assert.hpp"
#include "../fixture/host_abi_test_values.hpp"
#include "../runner/host_abi_test_cases.hpp"

#include <cstdint>

extern "C" uint64_t kivo_host_abi_c_contract_size(void);

namespace host_abi_test {

void c_header_is_usable() {
    HOST_ABI_ASSERT(kivo_host_abi_c_contract_size() > 0u);
    HOST_ABI_ASSERT(KIVO_AUDIO_ABI_VERSION_MAJOR == 1u);
}

void versioned_structs_accept_tail_extension() {
    struct ExtendedCreateInfo {
        kivo_audio_create_info_v1 base;
        uint64_t future_fields[4];
    };
    ExtendedCreateInfo extended{};
    extended.base = create_info();
    extended.base.struct_size = sizeof(extended);

    kivo_audio_handle handle = KIVO_AUDIO_INVALID_HANDLE;
    HOST_ABI_ASSERT(
        kivo_audio_create(&extended.base, &handle)
        == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(handle != KIVO_AUDIO_INVALID_HANDLE);

    auto capabilities =
        output_structure<kivo_audio_capabilities_v1>();
    capabilities.struct_size =
        sizeof(kivo_audio_capabilities_v1) + 64u;
    HOST_ABI_ASSERT(
        kivo_audio_get_capabilities(handle, &capabilities)
        == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(
        capabilities.abi_major == KIVO_AUDIO_ABI_VERSION_MAJOR);
    HOST_ABI_ASSERT(
        (capabilities.capability_flags
            & KIVO_AUDIO_CAPABILITY_HOST_SOURCE_CALLBACKS) != 0u);
    HOST_ABI_ASSERT(
        kivo_audio_destroy(handle) == KIVO_AUDIO_RESULT_OK);
}

} // namespace host_abi_test
