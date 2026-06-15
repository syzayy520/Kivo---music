#include "local_callback_source.hpp"

#include <fstream>
#include <memory>

namespace kivo::hardware_test::host_abi {

namespace {

struct LocalSourceContext {
    std::ifstream stream;
};

kivo_audio_source_io_result read_source(
    void* opaque,
    uint8_t* destination,
    uint64_t capacity,
    uint64_t* bytes_read) {
    auto& context = *static_cast<LocalSourceContext*>(opaque);
    context.stream.read(
        reinterpret_cast<char*>(destination),
        static_cast<std::streamsize>(capacity));
    *bytes_read =
        static_cast<uint64_t>(context.stream.gcount());
    if (*bytes_read != 0u) {
        return KIVO_AUDIO_SOURCE_IO_DATA;
    }
    return context.stream.eof()
        ? KIVO_AUDIO_SOURCE_IO_END_OF_STREAM
        : KIVO_AUDIO_SOURCE_IO_FAILED;
}

kivo_audio_source_io_result seek_source(
    void* opaque,
    int64_t offset,
    kivo_audio_seek_origin origin,
    uint64_t* absolute_position) {
    auto& stream =
        static_cast<LocalSourceContext*>(opaque)->stream;
    stream.clear();
    std::ios_base::seekdir direction = std::ios_base::beg;
    if (origin == KIVO_AUDIO_SEEK_ORIGIN_CURRENT) {
        direction = std::ios_base::cur;
    } else if (origin == KIVO_AUDIO_SEEK_ORIGIN_END) {
        direction = std::ios_base::end;
    }
    stream.seekg(offset, direction);
    if (!stream) {
        return KIVO_AUDIO_SOURCE_IO_FAILED;
    }
    const auto position = stream.tellg();
    if (position < 0) {
        return KIVO_AUDIO_SOURCE_IO_FAILED;
    }
    *absolute_position = static_cast<uint64_t>(position);
    return KIVO_AUDIO_SOURCE_IO_DATA;
}

void release_source(void* opaque) {
    delete static_cast<LocalSourceContext*>(opaque);
}

} // namespace

kivo_audio_source_v1 make_local_callback_source(
    const std::filesystem::path& path) {
    auto context = std::make_unique<LocalSourceContext>();
    context->stream.open(path, std::ios::binary);
    if (!context->stream) {
        return {};
    }
    const auto size = std::filesystem::file_size(path);
    kivo_audio_source_v1 source{};
    source.struct_size = sizeof(source);
    source.struct_version = KIVO_AUDIO_ABI_STRUCT_VERSION_1;
    source.source_context = context.release();
    source.source_identity = 1;
    source.source_generation = 1;
    source.size_bytes = size;
    source.flags = KIVO_AUDIO_SOURCE_FLAG_SEEKABLE
        | KIVO_AUDIO_SOURCE_FLAG_SIZE_KNOWN;
    source.read = &read_source;
    source.seek = &seek_source;
    source.release = &release_source;
    return source;
}

} // namespace kivo::hardware_test::host_abi
