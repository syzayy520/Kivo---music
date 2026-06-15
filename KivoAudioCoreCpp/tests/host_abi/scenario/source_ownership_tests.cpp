#include "../fixture/host_abi_test_assert.hpp"
#include "../fixture/host_abi_test_values.hpp"
#include "../runner/host_abi_test_cases.hpp"

namespace host_abi_test {

namespace {

struct SourceContext {
    uint32_t release_count{0};
};

kivo_audio_source_io_result read_source(
    void*,
    uint8_t*,
    uint64_t,
    uint64_t* bytes_read) {
    *bytes_read = 0;
    return KIVO_AUDIO_SOURCE_IO_END_OF_STREAM;
}

void release_source(void* context) {
    ++static_cast<SourceContext*>(context)->release_count;
}

} // namespace

void accepted_source_ownership_releases_once() {
    auto info = create_info();
    kivo_audio_handle handle = KIVO_AUDIO_INVALID_HANDLE;
    HOST_ABI_ASSERT(
        kivo_audio_create(&info, &handle) == KIVO_AUDIO_RESULT_OK);

    SourceContext context{};
    auto source = output_structure<kivo_audio_source_v1>();
    source.source_context = &context;
    source.source_identity = 71;
    source.source_generation = 9;
    source.read = &read_source;
    source.release = &release_source;

    auto configuration = invalid_open_configuration();
    HOST_ABI_ASSERT(
        kivo_audio_open_source(
            handle,
            1,
            1,
            &source,
            &configuration)
        == KIVO_AUDIO_RESULT_INVALID_ARGUMENT);
    HOST_ABI_ASSERT(context.release_count == 1u);
    HOST_ABI_ASSERT(
        kivo_audio_destroy(handle) == KIVO_AUDIO_RESULT_OK);
    HOST_ABI_ASSERT(context.release_count == 1u);
}

} // namespace host_abi_test
