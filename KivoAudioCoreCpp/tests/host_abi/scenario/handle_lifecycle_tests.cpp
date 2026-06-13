#include "../fixture/host_abi_test_assert.hpp"
#include "../fixture/host_abi_test_values.hpp"
#include "../runner/host_abi_test_cases.hpp"

namespace host_abi_test {

void invalid_and_stale_handles_are_rejected() {
    auto snapshot = output_structure<kivo_audio_snapshot_v1>();
    HOST_ABI_ASSERT(
        kivo_audio_get_snapshot(0xBADF00Du, &snapshot)
        == KIVO_AUDIO_RESULT_INVALID_HANDLE);

    auto info = create_info();
    kivo_audio_handle handle = KIVO_AUDIO_INVALID_HANDLE;
    HOST_ABI_ASSERT(
        kivo_audio_create(&info, &handle) == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(
        kivo_audio_destroy(handle) == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(
        kivo_audio_get_snapshot(handle, &snapshot)
        == KIVO_AUDIO_RESULT_INVALID_HANDLE);
    HOST_ABI_ASSERT(
        kivo_audio_destroy(handle)
        == KIVO_AUDIO_RESULT_INVALID_HANDLE);
}

void shutdown_is_terminal_and_destroy_remains_valid() {
    auto info = create_info();
    kivo_audio_handle handle = KIVO_AUDIO_INVALID_HANDLE;
    HOST_ABI_ASSERT(
        kivo_audio_create(&info, &handle) == KIVO_AUDIO_RESULT_OK);

    auto shutdown = output_structure<kivo_audio_command_v1>();
    shutdown.command_id = 1;
    shutdown.kind = KIVO_AUDIO_COMMAND_SHUTDOWN;
    HOST_ABI_ASSERT(
        kivo_audio_submit_command(handle, &shutdown)
        == KIVO_AUDIO_RESULT_OK);

    auto repeated = shutdown;
    repeated.command_id = 2;
    HOST_ABI_ASSERT(
        kivo_audio_submit_command(handle, &repeated)
        == KIVO_AUDIO_RESULT_SHUTDOWN);

    auto snapshot = output_structure<kivo_audio_snapshot_v1>();
    HOST_ABI_ASSERT(
        kivo_audio_get_snapshot(handle, &snapshot)
        == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(snapshot.state == KIVO_AUDIO_STATE_CLOSED);
    HOST_ABI_ASSERT(
        kivo_audio_destroy(handle) == KIVO_AUDIO_RESULT_OK);
}

} // namespace host_abi_test
