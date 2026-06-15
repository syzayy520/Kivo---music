#include "../fixture/runtime_test_assert.hpp"
#include "../fixture/runtime_test_call_gate.hpp"
#include "../fixture/runtime_test_decoder.hpp"
#include "../fixture/runtime_test_factory.hpp"
#include "../fixture/runtime_test_gated_renderer.hpp"
#include "../runner/playback_runtime_test_cases.hpp"

#include <atomic>
#include <latch>
#include <thread>

namespace playback_runtime_test {

void shutdown_waits_for_active_write_and_releases_runtime() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    ScriptedDecoder decoder;
    testing::time::ManualClock clock;
    RuntimeTestCallGate write_gate;
    const auto configuration = renderer_configuration();
    RuntimeTestGatedRenderer renderer{
        configuration,
        clock,
        &write_gate};
    core::playback::PlaybackRuntimeCoordinator runtime{
        session,
        decoder,
        renderer};

    RUNTIME_ASSERT(runtime.open(
        command(1, core::contract::CommandKind::OpenSource, 23),
        source(51, 24),
        open_request(configuration, 25)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(2, core::contract::CommandKind::Resume, 23)).succeeded());
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);

    core::render::RenderPumpResult render_result{};
    std::thread render_thread{[&] {
        render_result = runtime.render_step();
    }};
    write_gate.wait_until_entered();

    core::playback::PlaybackRuntimeResult shutdown_result{};
    std::atomic<bool> shutdown_completed{false};
    std::latch shutdown_started{1};
    std::thread shutdown_thread{[&] {
        shutdown_started.count_down();
        shutdown_result = runtime.execute(
            command(3, core::contract::CommandKind::Shutdown, 23));
        shutdown_completed.store(true);
    }};
    shutdown_started.wait();
    RUNTIME_ASSERT(!shutdown_completed.load());

    write_gate.release();
    render_thread.join();
    shutdown_thread.join();

    RUNTIME_ASSERT(
        render_result.disposition
        == core::render::RenderPumpDisposition::EndOfStream);
    RUNTIME_ASSERT(shutdown_result.succeeded());
    const auto closed = runtime.snapshot();
    RUNTIME_ASSERT(!closed.active);
    RUNTIME_ASSERT(
        closed.session.state == core::contract::CoreState::Closed);
    RUNTIME_ASSERT(
        closed.renderer.state
        == core::render::RenderLifecycleState::Closed);
    RUNTIME_ASSERT(decoder.close_count == 1);
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Failed);
    RUNTIME_ASSERT(
        runtime.execute(
            command(4, core::contract::CommandKind::Shutdown, 0)).failure
        == core::playback::PlaybackRuntimeFailure::RuntimeClosed);
}

void close_waits_for_active_drain_and_releases_runtime() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    ScriptedDecoder decoder;
    testing::time::ManualClock clock;
    RuntimeTestCallGate drain_gate;
    const auto configuration = renderer_configuration();
    RuntimeTestGatedRenderer renderer{
        configuration,
        clock,
        nullptr,
        &drain_gate};
    core::playback::PlaybackRuntimeCoordinator runtime{
        session,
        decoder,
        renderer};

    RUNTIME_ASSERT(runtime.open(
        command(1, core::contract::CommandKind::OpenSource, 31),
        source(52, 32),
        open_request(configuration, 33)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(2, core::contract::CommandKind::Resume, 31)).succeeded());
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::Primed);
    RUNTIME_ASSERT(
        runtime.produce_step().disposition
        == core::playback::DecodeRenderQueueProducerDisposition::EndOfStream);

    core::render::RenderPumpResult render_result{};
    std::thread render_thread{[&] {
        render_result = runtime.render_step();
    }};
    drain_gate.wait_until_entered();

    core::playback::PlaybackRuntimeResult close_result{};
    std::atomic<bool> close_completed{false};
    std::latch close_started{1};
    std::thread close_thread{[&] {
        close_started.count_down();
        close_result = runtime.execute(
            command(3, core::contract::CommandKind::CloseSource, 31));
        close_completed.store(true);
    }};
    close_started.wait();
    RUNTIME_ASSERT(!close_completed.load());

    drain_gate.release();
    render_thread.join();
    close_thread.join();

    RUNTIME_ASSERT(
        render_result.disposition
        == core::render::RenderPumpDisposition::EndOfStream);
    RUNTIME_ASSERT(close_result.succeeded());
    const auto closed = runtime.snapshot();
    RUNTIME_ASSERT(!closed.active);
    RUNTIME_ASSERT(
        closed.renderer.state
        == core::render::RenderLifecycleState::Closed);
    RUNTIME_ASSERT(decoder.close_count == 1);
}

} // namespace playback_runtime_test
