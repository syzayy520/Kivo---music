#include "../state/decode_render_queue_producer_state.hpp"

namespace kivo::core::playback {

DecodeRenderQueueProducerResult
DecodeRenderQueueProducer::Impl::step() noexcept {
    if (snapshot_.state == DecodeRenderQueueProducerState::Failed) {
        return failed(snapshot_.last_decode_failure);
    }
    if (snapshot_.state == DecodeRenderQueueProducerState::EndOfStream) {
        return {
            DecodeRenderQueueProducerDisposition::EndOfStream,
            decode::DecodeFailure::None,
            render::SpscQueuePushResult::Closed,
            0
        };
    }
    if (snapshot_.state
        == DecodeRenderQueueProducerState::PrefetchedEndOfStream) {
        return finish();
    }
    if (snapshot_.state == DecodeRenderQueueProducerState::Ready) {
        const auto result = prefetch();
        if (snapshot_.state
            != DecodeRenderQueueProducerState::PrefetchedEndOfStream) {
            return result;
        }
        return finish();
    }
    snapshot_.state = DecodeRenderQueueProducerState::Running;

    if (active_index_) {
        return enqueue_active();
    }
    if (held_index_ && held_is_terminal_) {
        activate_held(true);
        return enqueue_active();
    }

    const auto decoded = decoder_.decode_next();
    switch (decoded.disposition()) {
    case decode::DecodeStepDisposition::Produced:
        switch (store_decoded(decoded.block())) {
        case pipeline_buffer::StoreResult::DiscardedStale:
            return {
                DecodeRenderQueueProducerDisposition::DiscardedStale,
                snapshot_.last_decode_failure,
                render::SpscQueuePushResult::Pushed,
                0
            };
        case pipeline_buffer::StoreResult::Failed:
            return failed(snapshot_.last_decode_failure);
        case pipeline_buffer::StoreResult::Stored:
            break;
        }
        if (!held_index_) {
            held_index_ = stored_index_;
            held_is_terminal_ = decoded.block().end_of_stream;
            return {
                DecodeRenderQueueProducerDisposition::Primed,
                decode::DecodeFailure::None,
                render::SpscQueuePushResult::Pushed,
                0
            };
        }
        active_index_ = held_index_;
        active_cursor_ = 0;
        active_is_terminal_ = false;
        held_index_ = stored_index_;
        held_is_terminal_ = decoded.block().end_of_stream;
        return enqueue_active();

    case decode::DecodeStepDisposition::EndOfStream:
        if (held_index_) {
            activate_held(true);
            return enqueue_active();
        }
        return finish();

    case decode::DecodeStepDisposition::Failed:
        return failed(decoded.failure());
    }
    return failed(decode::DecodeFailure::BoundaryFailure);
}

DecodeRenderQueueProducerResult
DecodeRenderQueueProducer::Impl::prefetch() noexcept {
    if (snapshot_.state == DecodeRenderQueueProducerState::Failed) {
        return failed(snapshot_.last_decode_failure);
    }
    if (snapshot_.state == DecodeRenderQueueProducerState::EndOfStream
        || snapshot_.state
            == DecodeRenderQueueProducerState::PrefetchedEndOfStream) {
        return {
            DecodeRenderQueueProducerDisposition::EndOfStream,
            decode::DecodeFailure::None,
            render::SpscQueuePushResult::Pushed,
            0
        };
    }
    if (snapshot_.state != DecodeRenderQueueProducerState::Ready) {
        return {
            DecodeRenderQueueProducerDisposition::Primed,
            decode::DecodeFailure::None,
            render::SpscQueuePushResult::Pushed,
            0
        };
    }

    const auto decoded = decoder_.decode_next();
    if (decoded.disposition() == decode::DecodeStepDisposition::Failed) {
        return failed(decoded.failure());
    }
    if (decoded.disposition() == decode::DecodeStepDisposition::EndOfStream) {
        snapshot_.state =
            DecodeRenderQueueProducerState::PrefetchedEndOfStream;
        return {
            DecodeRenderQueueProducerDisposition::EndOfStream,
            decode::DecodeFailure::None,
            render::SpscQueuePushResult::Pushed,
            0
        };
    }
    if (store_decoded(decoded.block())
        != pipeline_buffer::StoreResult::Stored) {
        return snapshot_.state == DecodeRenderQueueProducerState::Failed
            ? failed(snapshot_.last_decode_failure)
            : DecodeRenderQueueProducerResult{
                DecodeRenderQueueProducerDisposition::DiscardedStale,
                snapshot_.last_decode_failure,
                render::SpscQueuePushResult::Pushed,
                0};
    }
    held_index_ = stored_index_;
    held_is_terminal_ = decoded.block().end_of_stream;
    snapshot_.state = DecodeRenderQueueProducerState::Running;
    return {
        DecodeRenderQueueProducerDisposition::Primed,
        decode::DecodeFailure::None,
        render::SpscQueuePushResult::Pushed,
        0
    };
}

DecodeRenderQueueProducerResult
DecodeRenderQueueProducer::Impl::finish() noexcept {
    if (configuration_.end_of_stream_policy
        == QueueEndOfStreamPolicy::CloseAndMarkFinal) {
        queue_.close_producer();
    }
    snapshot_.state = DecodeRenderQueueProducerState::EndOfStream;
    return {
        DecodeRenderQueueProducerDisposition::EndOfStream,
        decode::DecodeFailure::None,
        configuration_.end_of_stream_policy
                == QueueEndOfStreamPolicy::CloseAndMarkFinal
            ? render::SpscQueuePushResult::Closed
            : render::SpscQueuePushResult::Pushed,
        0
    };
}

DecodeRenderQueueProducerSnapshot
DecodeRenderQueueProducer::Impl::snapshot() const noexcept {
    auto result = snapshot_;
    if (processing_) {
        result.processing = processing_->snapshot();
    }
    return result;
}

DecodeRenderQueueProducerResult
DecodeRenderQueueProducer::Impl::failed(
    decode::DecodeFailure failure) noexcept {
    snapshot_.state = DecodeRenderQueueProducerState::Failed;
    snapshot_.last_decode_failure =
        failure == decode::DecodeFailure::None
        ? decode::DecodeFailure::BoundaryFailure
        : failure;
    return {
        DecodeRenderQueueProducerDisposition::Failed,
        snapshot_.last_decode_failure,
        render::SpscQueuePushResult::Closed,
        0
    };
}

} // namespace kivo::core::playback
