#include "kivo/source/local/local_file_byte_source.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <system_error>

namespace kivo::source::local {

class LocalFileByteSource::Impl {
public:
    Impl(
        const std::filesystem::path& path,
        core::contract::SourceIdentity identity,
        core::contract::SourceGeneration generation)
        : stream(path, std::ios::binary),
          identity(identity),
          generation(generation) {
        std::error_code error;
        const auto file_size = std::filesystem::file_size(path, error);
        if (!error) {
            size = file_size;
        }
    }

    std::ifstream stream;
    core::contract::SourceIdentity identity{};
    core::contract::SourceGeneration generation{};
    std::optional<uint64_t> size{};
};

std::unique_ptr<LocalFileByteSource> LocalFileByteSource::open(
    const std::filesystem::path& path,
    core::contract::SourceIdentity identity,
    core::contract::SourceGeneration generation) noexcept {
    if (path.empty() || identity.value == 0) {
        return {};
    }
    try {
        auto impl = std::make_unique<Impl>(path, identity, generation);
        if (!impl->stream.is_open()) {
            return {};
        }
        return std::unique_ptr<LocalFileByteSource>{
            new LocalFileByteSource(std::move(impl))};
    } catch (...) {
        return {};
    }
}

LocalFileByteSource::LocalFileByteSource(std::unique_ptr<Impl> impl) noexcept
    : impl_(std::move(impl)) {}

LocalFileByteSource::~LocalFileByteSource() = default;

core::contract::SourceIdentity LocalFileByteSource::identity() const noexcept {
    return impl_ ? impl_->identity : core::contract::SourceIdentity{};
}

core::contract::SourceGeneration LocalFileByteSource::generation() const noexcept {
    return impl_ ? impl_->generation : core::contract::SourceGeneration{};
}

bool LocalFileByteSource::seekable() const noexcept {
    return impl_ && impl_->stream.is_open();
}

std::optional<uint64_t> LocalFileByteSource::size_bytes() const noexcept {
    return impl_ ? impl_->size : std::nullopt;
}

core::decode::ByteSourceReadResult LocalFileByteSource::read(
    std::span<std::byte> destination) noexcept {
    using namespace core::decode;
    if (!impl_ || !impl_->stream.is_open() || destination.empty()) {
        return {ByteSourceReadDisposition::Failed, 0, DecodeFailure::InvalidBuffer};
    }
    const auto maximum =
        static_cast<size_t>(std::numeric_limits<std::streamsize>::max());
    const auto requested = static_cast<std::streamsize>(
        std::min(destination.size(), maximum));
    impl_->stream.read(
        reinterpret_cast<char*>(destination.data()),
        requested);
    const auto count = impl_->stream.gcount();
    if (count > 0) {
        return {
            ByteSourceReadDisposition::Data,
            static_cast<size_t>(count),
            DecodeFailure::None
        };
    }
    if (impl_->stream.eof()) {
        return {
            ByteSourceReadDisposition::EndOfStream,
            0,
            DecodeFailure::EndOfStream
        };
    }
    return {
        ByteSourceReadDisposition::Failed,
        0,
        DecodeFailure::SourceReadFailed
    };
}

core::decode::ByteSourceSeekResult LocalFileByteSource::seek(
    int64_t offset,
    core::decode::ByteSeekOrigin origin) noexcept {
    using namespace core::decode;
    if (!impl_ || !impl_->stream.is_open()) {
        return {
            ByteSourceSeekDisposition::Failed,
            0,
            DecodeFailure::SourceSeekFailed
        };
    }
    std::ios::seekdir direction = std::ios::beg;
    switch (origin) {
    case ByteSeekOrigin::Begin: direction = std::ios::beg; break;
    case ByteSeekOrigin::Current: direction = std::ios::cur; break;
    case ByteSeekOrigin::End: direction = std::ios::end; break;
    }

    impl_->stream.clear();
    impl_->stream.seekg(static_cast<std::streamoff>(offset), direction);
    if (!impl_->stream) {
        impl_->stream.clear();
        return {
            ByteSourceSeekDisposition::Failed,
            0,
            DecodeFailure::SourceSeekFailed
        };
    }
    const auto position = impl_->stream.tellg();
    if (position < 0) {
        return {
            ByteSourceSeekDisposition::Failed,
            0,
            DecodeFailure::SourceSeekFailed
        };
    }
    return {
        ByteSourceSeekDisposition::Succeeded,
        static_cast<uint64_t>(position),
        DecodeFailure::None
    };
}

} // namespace kivo::source::local
