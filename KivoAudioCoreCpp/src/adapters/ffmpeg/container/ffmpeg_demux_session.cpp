#include "adapters/ffmpeg/container/ffmpeg_demux_session.hpp"

#include <limits>

#include "adapters/ffmpeg/source/ffmpeg_avio_bridge.hpp"

extern "C" {
#include <libavutil/error.h>
}

namespace kivo::adapters::ffmpeg::detail {

FfmpegDemuxSession::~FfmpegDemuxSession() {
    close();
}

bool FfmpegDemuxSession::open(FfmpegAvioBridge& bridge) noexcept {
    close();
    context_ = avformat_alloc_context();
    if (context_ == nullptr || bridge.context() == nullptr) {
        failure_ = core::decode::DecodeFailure::BoundaryFailure;
        close();
        return false;
    }
    context_->pb = bridge.context();
    context_->flags |= AVFMT_FLAG_CUSTOM_IO;

    if (avformat_open_input(&context_, nullptr, nullptr, nullptr) < 0) {
        failure_ = bridge.last_failure() != core::decode::DecodeFailure::None
            ? bridge.last_failure()
            : core::decode::DecodeFailure::UnsupportedContainer;
        close();
        return false;
    }
    if (avformat_find_stream_info(context_, nullptr) < 0) {
        failure_ = core::decode::DecodeFailure::InvalidMediaData;
        close();
        return false;
    }
    audio_stream_index_ = av_find_best_stream(
        context_,
        AVMEDIA_TYPE_AUDIO,
        -1,
        -1,
        nullptr,
        0);
    if (audio_stream_index_ < 0) {
        failure_ = core::decode::DecodeFailure::NoAudioStream;
        close();
        return false;
    }
    failure_ = core::decode::DecodeFailure::None;
    return true;
}

void FfmpegDemuxSession::close() noexcept {
    if (context_ != nullptr) {
        avformat_close_input(&context_);
    }
    audio_stream_index_ = -1;
}

int FfmpegDemuxSession::read_audio_packet(AVPacket* packet) noexcept {
    if (context_ == nullptr || packet == nullptr || audio_stream_index_ < 0) {
        failure_ = core::decode::DecodeFailure::BoundaryFailure;
        return AVERROR(EINVAL);
    }
    for (;;) {
        const auto result = av_read_frame(context_, packet);
        if (result < 0) {
            if (result != AVERROR_EOF) {
                failure_ = core::decode::DecodeFailure::InvalidMediaData;
            }
            return result;
        }
        if (packet->stream_index == audio_stream_index_) {
            return 0;
        }
        av_packet_unref(packet);
    }
}

bool FfmpegDemuxSession::seek_to(int64_t stream_timestamp) noexcept {
    if (context_ == nullptr || audio_stream_index_ < 0) {
        failure_ = core::decode::DecodeFailure::BoundaryFailure;
        return false;
    }
    const auto result = avformat_seek_file(
        context_,
        audio_stream_index_,
        std::numeric_limits<int64_t>::min(),
        stream_timestamp,
        stream_timestamp,
        AVSEEK_FLAG_BACKWARD);
    if (result < 0) {
        failure_ = core::decode::DecodeFailure::SourceSeekFailed;
        return false;
    }
    avformat_flush(context_);
    failure_ = core::decode::DecodeFailure::None;
    return true;
}

AVStream* FfmpegDemuxSession::audio_stream() const noexcept {
    if (context_ == nullptr
        || audio_stream_index_ < 0
        || audio_stream_index_ >= static_cast<int>(context_->nb_streams)) {
        return nullptr;
    }
    return context_->streams[audio_stream_index_];
}

AVFormatContext* FfmpegDemuxSession::context() const noexcept {
    return context_;
}

int FfmpegDemuxSession::audio_stream_index() const noexcept {
    return audio_stream_index_;
}

core::decode::DecodeFailure FfmpegDemuxSession::failure() const noexcept {
    return failure_;
}

} // namespace kivo::adapters::ffmpeg::detail
