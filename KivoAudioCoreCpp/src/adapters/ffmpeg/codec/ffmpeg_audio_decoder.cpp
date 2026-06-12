#include "adapters/ffmpeg/codec/ffmpeg_audio_decoder.hpp"

namespace kivo::adapters::ffmpeg::detail {

FfmpegAudioDecoder::~FfmpegAudioDecoder() {
    close();
}

bool FfmpegAudioDecoder::open(
    const AVCodecParameters& parameters) noexcept {
    close();
    const auto* codec = avcodec_find_decoder(parameters.codec_id);
    if (codec == nullptr) {
        failure_ = core::decode::DecodeFailure::UnsupportedCodec;
        return false;
    }
    context_ = avcodec_alloc_context3(codec);
    packet_ = av_packet_alloc();
    frame_ = av_frame_alloc();
    if (context_ == nullptr || packet_ == nullptr || frame_ == nullptr) {
        failure_ = core::decode::DecodeFailure::BoundaryFailure;
        close();
        return false;
    }
    if (avcodec_parameters_to_context(context_, &parameters) < 0
        || avcodec_open2(context_, codec, nullptr) < 0) {
        failure_ = core::decode::DecodeFailure::DecoderOpenFailed;
        close();
        return false;
    }
    failure_ = core::decode::DecodeFailure::None;
    return true;
}

void FfmpegAudioDecoder::close() noexcept {
    av_frame_free(&frame_);
    av_packet_free(&packet_);
    avcodec_free_context(&context_);
}

int FfmpegAudioDecoder::send_packet(const AVPacket* packet) noexcept {
    if (context_ == nullptr) {
        failure_ = core::decode::DecodeFailure::BoundaryFailure;
        return AVERROR(EINVAL);
    }
    const auto result = avcodec_send_packet(context_, packet);
    if (result < 0 && result != AVERROR(EAGAIN) && result != AVERROR_EOF) {
        failure_ = core::decode::DecodeFailure::InvalidMediaData;
    }
    return result;
}

int FfmpegAudioDecoder::receive_frame() noexcept {
    if (context_ == nullptr || frame_ == nullptr) {
        failure_ = core::decode::DecodeFailure::BoundaryFailure;
        return AVERROR(EINVAL);
    }
    av_frame_unref(frame_);
    const auto result = avcodec_receive_frame(context_, frame_);
    if (result < 0 && result != AVERROR(EAGAIN) && result != AVERROR_EOF) {
        failure_ = core::decode::DecodeFailure::InvalidMediaData;
    }
    return result;
}

void FfmpegAudioDecoder::unref_packet() noexcept {
    if (packet_ != nullptr) {
        av_packet_unref(packet_);
    }
}

void FfmpegAudioDecoder::unref_frame() noexcept {
    if (frame_ != nullptr) {
        av_frame_unref(frame_);
    }
}

void FfmpegAudioDecoder::flush() noexcept {
    if (context_ != nullptr) {
        avcodec_flush_buffers(context_);
    }
    unref_packet();
    unref_frame();
    failure_ = core::decode::DecodeFailure::None;
}

AVPacket* FfmpegAudioDecoder::packet() const noexcept {
    return packet_;
}

AVFrame* FfmpegAudioDecoder::frame() const noexcept {
    return frame_;
}

AVCodecContext* FfmpegAudioDecoder::context() const noexcept {
    return context_;
}

core::decode::DecodeFailure FfmpegAudioDecoder::failure() const noexcept {
    return failure_;
}

} // namespace kivo::adapters::ffmpeg::detail
