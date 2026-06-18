#include "../fixture/host_abi_test_assert.hpp"
#include "../fixture/host_abi_test_values.hpp"
#include "../runner/host_abi_test_cases.hpp"

#include <cstdint>

extern "C" uint64_t kivo_host_abi_c_contract_size(void);

namespace host_abi_test {
namespace {

struct LegacySnapshotV1 {
    uint32_t struct_size;
    uint32_t struct_version;
    uint32_t state;
    uint32_t active;
    uint64_t session_generation;
    uint64_t last_command_id;
    uint64_t rendered_position_frames;
    uint64_t decoded_frames;
    uint64_t queued_frames;
    uint64_t successful_opens;
    uint64_t successful_seeks;
    uint64_t failed_operations;
    uint64_t stale_blocks_rejected;
    uint64_t device_loss_events;
    uint64_t decode_failure_events;
    kivo_audio_result last_result;
    uint32_t reserved;
};

static_assert(
    sizeof(LegacySnapshotV1) == KIVO_AUDIO_SNAPSHOT_V1_BASE_SIZE);

[[nodiscard]] kivo_audio_handle create_engine() {
    kivo_audio_handle handle = KIVO_AUDIO_INVALID_HANDLE;
    const auto info = create_info();
    HOST_ABI_ASSERT(
        kivo_audio_create(&info, &handle) == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(handle != KIVO_AUDIO_INVALID_HANDLE);
    return handle;
}

} // namespace

void c_header_is_usable() {
    HOST_ABI_ASSERT(kivo_host_abi_c_contract_size() > 0u);
    // Compile-time invariant: assert it as such (runtime-asserting a constant
    // trips MSVC C4127 under /W4 /WX). Matches the static_assert at line 33.
    static_assert(KIVO_AUDIO_ABI_VERSION_MAJOR == 1u);
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

void legacy_snapshot_prefix_is_not_overwritten() {
    struct GuardedLegacySnapshot {
        LegacySnapshotV1 snapshot{};
        uint64_t guard_a{0x0123456789abcdefull};
        uint64_t guard_b{0xfedcba9876543210ull};
    };

    const auto handle = create_engine();
    GuardedLegacySnapshot guarded{};
    guarded.snapshot.struct_size = sizeof(guarded.snapshot);
    guarded.snapshot.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;

    HOST_ABI_ASSERT(
        kivo_audio_get_snapshot(
            handle,
            reinterpret_cast<kivo_audio_snapshot_v1*>(
                &guarded.snapshot))
        == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(
        guarded.snapshot.struct_size == sizeof(kivo_audio_snapshot_v1));
    HOST_ABI_ASSERT(guarded.guard_a == 0x0123456789abcdefull);
    HOST_ABI_ASSERT(guarded.guard_b == 0xfedcba9876543210ull);
    HOST_ABI_ASSERT(
        kivo_audio_destroy(handle) == KIVO_AUDIO_RESULT_OK);
}

void extended_diagnostic_snapshot_is_advertised() {
    const auto handle = create_engine();
    auto capabilities =
        output_structure<kivo_audio_capabilities_v1>();
    HOST_ABI_ASSERT(
        kivo_audio_get_capabilities(handle, &capabilities)
        == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(
        (capabilities.capability_flags
            & KIVO_AUDIO_CAPABILITY_EXTENDED_DIAGNOSTIC_SNAPSHOT)
        != 0u);

    auto snapshot = output_structure<kivo_audio_snapshot_v1>();
    HOST_ABI_ASSERT(
        kivo_audio_get_snapshot(handle, &snapshot)
        == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(
        snapshot.struct_size == sizeof(kivo_audio_snapshot_v1));
    HOST_ABI_ASSERT(snapshot.format_negotiation_attempts == 0u);
    HOST_ABI_ASSERT(snapshot.device_reopen_attempts == 0u);
    HOST_ABI_ASSERT(snapshot.render_underrun_events == 0u);
    HOST_ABI_ASSERT(
        kivo_audio_destroy(handle) == KIVO_AUDIO_RESULT_OK);
}

void bit_perfect_truth_is_advertised_and_honest_when_idle() {
    const auto handle = create_engine();

    auto capabilities = output_structure<kivo_audio_capabilities_v1>();
    HOST_ABI_ASSERT(
        kivo_audio_get_capabilities(handle, &capabilities)
        == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(
        (capabilities.capability_flags
            & KIVO_AUDIO_CAPABILITY_BIT_PERFECT_TRUTH) != 0u);

    // No source open => no evidence => must report EVIDENCE_INCOMPLETE and must
    // NEVER claim bit-perfect, and modes/flags stay Unknown/absent.
    auto truth = output_structure<kivo_audio_truth_v1>();
    HOST_ABI_ASSERT(
        kivo_audio_get_truth(handle, &truth) == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(truth.struct_size == sizeof(kivo_audio_truth_v1));
    HOST_ABI_ASSERT(truth.evidence_complete == 0u);
    HOST_ABI_ASSERT(
        truth.verdict
        == KIVO_AUDIO_BIT_PERFECT_VERDICT_EVIDENCE_INCOMPLETE);
    HOST_ABI_ASSERT(
        truth.verdict != KIVO_AUDIO_BIT_PERFECT_VERDICT_BIT_PERFECT);
    HOST_ABI_ASSERT(truth.requested_mode == KIVO_AUDIO_TRUTH_MODE_UNKNOWN);
    HOST_ABI_ASSERT(truth.actual_mode == KIVO_AUDIO_TRUTH_MODE_UNKNOWN);
    HOST_ABI_ASSERT(truth.known_mask == 0u);
    HOST_ABI_ASSERT(
        kivo_audio_destroy(handle) == KIVO_AUDIO_RESULT_OK);
}

void truth_getter_rejects_invalid_struct() {
    const auto handle = create_engine();
    kivo_audio_truth_v1 truth{};  // struct_size 0 => below base size
    HOST_ABI_ASSERT(
        kivo_audio_get_truth(handle, &truth)
        == KIVO_AUDIO_RESULT_INVALID_STRUCT);
    HOST_ABI_ASSERT(
        kivo_audio_get_truth(handle, nullptr)
        == KIVO_AUDIO_RESULT_INVALID_STRUCT);
    HOST_ABI_ASSERT(
        kivo_audio_destroy(handle) == KIVO_AUDIO_RESULT_OK);
}

} // namespace host_abi_test
