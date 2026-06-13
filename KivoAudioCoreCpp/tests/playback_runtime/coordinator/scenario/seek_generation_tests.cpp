#include "../fixture/runtime_test_assert.hpp"
#include "../fixture/runtime_test_decoder.hpp"
#include "../fixture/runtime_test_factory.hpp"
#include "../runner/playback_runtime_test_cases.hpp"

namespace playback_runtime_test {

void seek_resets_all_scopes_and_discards_old_audio() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    ScriptedDecoder decoder;
    decoder.emit_stale_after_seek = true;
    testing::time::ManualClock clock;
    const auto configuration = renderer_configuration();
    testing::render::FakeRenderer renderer{configuration, clock};
    core::playback::PlaybackRuntimeCoordinator runtime{
        session,
        decoder,
        renderer};

    RUNTIME_ASSERT(runtime.open(
        command(1, core::contract::CommandKind::OpenSource, 1),
        source(12, 2),
        open_request(configuration, 3)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(2, core::contract::CommandKind::Resume, 1)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(3, core::contract::CommandKind::Pause, 1)).succeeded());
    RUNTIME_ASSERT(session.snapshot().paused_position == 0);
    RUNTIME_ASSERT(runtime.seek(
        command(4, core::contract::CommandKind::Seek, 1, 100)).succeeded());

    const auto after_seek = runtime.snapshot();
    RUNTIME_ASSERT(
        after_seek.session.state == core::contract::CoreState::Paused);
    RUNTIME_ASSERT(after_seek.session.rendered_position == 100);
    RUNTIME_ASSERT(after_seek.session.paused_position == 100);
    RUNTIME_ASSERT(after_seek.decode_generation.value() == 4);
    RUNTIME_ASSERT(after_seek.render_generations.seek.value() == 1);
    RUNTIME_ASSERT(after_seek.render_generations.flush.value() == 1);
    RUNTIME_ASSERT(after_seek.queue.used_slots == 0);
    RUNTIME_ASSERT(after_seek.queue.queued_frames == 0);

    const auto stale = runtime.produce_step();
    RUNTIME_ASSERT(
        stale.disposition
        == core::playback::DecodeRenderQueueProducerDisposition::DiscardedStale);
    RUNTIME_ASSERT(
        stale.decode_failure
        == core::decode::DecodeFailure::StaleDecodeGeneration);
    RUNTIME_ASSERT(runtime.snapshot().stale_blocks_rejected == 1);
    RUNTIME_ASSERT(
        session.snapshot().state == core::contract::CoreState::Paused);
    RUNTIME_ASSERT(runtime.snapshot().queue.used_slots == 0);
    RUNTIME_ASSERT(runtime.snapshot().queue.queued_frames == 0);

    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);
    RUNTIME_ASSERT(runtime.execute(
        command(5, core::contract::CommandKind::Resume, 1)).succeeded());
    RUNTIME_ASSERT(
        runtime.render_step().disposition
        == core::render::RenderPumpDisposition::EndOfStream);
    RUNTIME_ASSERT(session.snapshot().rendered_position == 104);
    RUNTIME_ASSERT(
        session.snapshot().state == core::contract::CoreState::Stopped);
}

} // namespace playback_runtime_test
