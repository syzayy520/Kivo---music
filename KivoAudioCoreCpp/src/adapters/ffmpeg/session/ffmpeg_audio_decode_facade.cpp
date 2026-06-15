#include "kivo/adapters/ffmpeg/session/ffmpeg_audio_decode_session.hpp"

#include <utility>

#include "adapters/ffmpeg/session/ffmpeg_audio_decode_runtime.hpp"

namespace kivo::adapters::ffmpeg {

FfmpegAudioDecodeSession::FfmpegAudioDecodeSession()
    : impl_(std::make_unique<detail::FfmpegAudioDecodeRuntime>()) {}

FfmpegAudioDecodeSession::~FfmpegAudioDecodeSession() = default;

core::decode::DecodeOpenResult FfmpegAudioDecodeSession::open(
    std::unique_ptr<core::decode::ByteSourceBoundary> source,
    const core::decode::DecodeOpenRequest& request) noexcept {
    if (!impl_) {
        return core::decode::DecodeOpenResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
    try {
        return impl_->open(std::move(source), request);
    } catch (...) {
        impl_->close();
        return core::decode::DecodeOpenResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
}

core::decode::DecodeStepResult
FfmpegAudioDecodeSession::decode_next() noexcept {
    if (!impl_) {
        return core::decode::DecodeStepResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
    try {
        return impl_->decode_next();
    } catch (...) {
        return core::decode::DecodeStepResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
}

core::decode::DecodeControlResult FfmpegAudioDecodeSession::seek(
    core::contract::SamplePosition target_frame,
    core::decode::DecodeGeneration generation) noexcept {
    if (!impl_) {
        return core::decode::DecodeControlResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
    try {
        return impl_->seek(target_frame, generation);
    } catch (...) {
        return core::decode::DecodeControlResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
}

core::decode::DecodeControlResult FfmpegAudioDecodeSession::flush(
    core::decode::DecodeGeneration generation) noexcept {
    if (!impl_) {
        return core::decode::DecodeControlResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
    try {
        return impl_->flush(generation);
    } catch (...) {
        return core::decode::DecodeControlResult::failed(
            core::decode::DecodeFailure::BoundaryFailure);
    }
}

core::decode::DecodeControlResult FfmpegAudioDecodeSession::close() noexcept {
    return impl_
        ? impl_->close()
        : core::decode::DecodeControlResult::no_op();
}

core::decode::MediaProbe FfmpegAudioDecodeSession::probe() const noexcept {
    return impl_ ? impl_->probe() : core::decode::MediaProbe{};
}

} // namespace kivo::adapters::ffmpeg
