#include "../../fixture/gapless_test_assert.hpp"
#include "../../fixture/gapless_test_factory.hpp"
#include "../../runner/playback_gapless_test_cases.hpp"

#include "kivo/core/playback/gapless/coordinator/gapless_transition_coordinator.hpp"

namespace playback_gapless_test {

void coordinator_prefetches_and_advances_without_queue_gap() {
    using namespace kivo;
    std::array<std::byte, 32> first{};
    std::array<std::byte, 32> second{};
    first.fill(std::byte{0x31});
    second.fill(std::byte{0x32});
    ScriptedDecoder first_decoder;
    ScriptedDecoder second_decoder;
    first_decoder.blocks[0] = decoded_block(first, 4);
    first_decoder.block_count = 1;
    second_decoder.blocks[0] = decoded_block(second, 4);
    second_decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({4, 32});
    GAPLESS_ASSERT(queue);
    auto coordinator = core::playback::GaplessTransitionCoordinator::create(
        *queue,
        prepare_request(first_decoder, plan(0, 4, 0), 1));
    GAPLESS_ASSERT(coordinator);

    const auto prepared = coordinator->prepare_next(
        prepare_request(second_decoder, plan(1, 4, 4, 48000, true), 2));
    GAPLESS_ASSERT(prepared.prepared);
    GAPLESS_ASSERT(second_decoder.next_block == 1);
    GAPLESS_ASSERT(coordinator->snapshot().next_track_prefetched);
    GAPLESS_ASSERT(
        coordinator->step().disposition
        == core::playback::GaplessCoordinatorDisposition::Primed);
    const auto advanced = coordinator->step();
    GAPLESS_ASSERT(
        advanced.disposition
        == core::playback::GaplessCoordinatorDisposition::TrackAdvanced);
    GAPLESS_ASSERT(advanced.current_track_index == 1);
    GAPLESS_ASSERT(coordinator->snapshot().completed_transitions == 1);
    GAPLESS_ASSERT(queue->producer_closed());

    auto block = queue->try_peek();
    GAPLESS_ASSERT(block && block->frame_offset == 0);
    GAPLESS_ASSERT(block->bytes.front() == std::byte{0x31});
    GAPLESS_ASSERT(!block->end_of_stream);
    GAPLESS_ASSERT(queue->consume_front(4));
    block = queue->try_peek();
    GAPLESS_ASSERT(block && block->frame_offset == 4);
    GAPLESS_ASSERT(block->bytes.front() == std::byte{0x32});
    GAPLESS_ASSERT(block->end_of_stream);
}

void coordinator_rejects_incompatible_prefetch() {
    using namespace kivo;
    const std::array<std::byte, 32> bytes{};
    ScriptedDecoder first_decoder;
    ScriptedDecoder next_decoder;
    first_decoder.blocks[0] = decoded_block(bytes, 4);
    first_decoder.block_count = 1;
    next_decoder.blocks[0] = decoded_block(bytes, 4);
    next_decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({4, 32});
    GAPLESS_ASSERT(queue);
    auto coordinator = core::playback::GaplessTransitionCoordinator::create(
        *queue,
        prepare_request(first_decoder, plan(0, 4, 0), 1));
    GAPLESS_ASSERT(coordinator);
    const auto rejected = coordinator->prepare_next(
        prepare_request(next_decoder, plan(1, 4, 5, 48000, true), 2));
    GAPLESS_ASSERT(!rejected.prepared);
    GAPLESS_ASSERT(
        rejected.failure
        == core::playback::GaplessPrepareFailure::TransitionRejected);
    GAPLESS_ASSERT(
        rejected.transition.rejection
        == core::playback::GaplessTransitionRejectionReason::TimelineGap);
    GAPLESS_ASSERT(next_decoder.next_block == 0);
}

void coordinator_rejects_successor_after_final_track() {
    using namespace kivo;
    const std::array<std::byte, 32> bytes{};
    ScriptedDecoder first_decoder;
    ScriptedDecoder next_decoder;
    first_decoder.blocks[0] = decoded_block(bytes, 4);
    first_decoder.block_count = 1;
    next_decoder.blocks[0] = decoded_block(bytes, 4);
    next_decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({4, 32});
    GAPLESS_ASSERT(queue);
    auto coordinator = core::playback::GaplessTransitionCoordinator::create(
        *queue,
        prepare_request(
            first_decoder,
            plan(0, 4, 0, 48000, true),
            1));
    GAPLESS_ASSERT(coordinator);
    const auto rejected = coordinator->prepare_next(
        prepare_request(
            next_decoder,
            plan(1, 4, 4, 48000, true),
            2));
    GAPLESS_ASSERT(!rejected.prepared);
    GAPLESS_ASSERT(
        rejected.failure
        == core::playback::GaplessPrepareFailure::CurrentTrackFinal);
    GAPLESS_ASSERT(next_decoder.next_block == 0);
}

void coordinator_requires_prefetched_non_final_successor() {
    using namespace kivo;
    const std::array<std::byte, 32> bytes{};
    ScriptedDecoder decoder;
    decoder.blocks[0] = decoded_block(bytes, 4);
    decoder.block_count = 1;
    auto queue = core::render::SpscAudioBlockQueue::create({2, 32});
    GAPLESS_ASSERT(queue);
    auto coordinator = core::playback::GaplessTransitionCoordinator::create(
        *queue,
        prepare_request(decoder, plan(0, 4, 0), 1));
    GAPLESS_ASSERT(coordinator);
    static_cast<void>(coordinator->step());
    const auto result = coordinator->step();
    GAPLESS_ASSERT(
        result.failure
        == core::playback::GaplessCoordinatorFailure::
            MissingPrefetchedTrack);
    GAPLESS_ASSERT(!queue->producer_closed());
}

} // namespace playback_gapless_test
