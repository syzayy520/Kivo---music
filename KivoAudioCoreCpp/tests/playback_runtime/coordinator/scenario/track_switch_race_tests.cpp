#include "../fixture/runtime_test_assert.hpp"
#include "../fixture/runtime_test_call_gate.hpp"
#include "../fixture/runtime_test_decoder.hpp"
#include "../fixture/runtime_test_factory.hpp"
#include "../runner/playback_runtime_test_cases.hpp"

#include <thread>

namespace playback_runtime_test {

void stale_pause_cannot_mutate_replaced_source() {
    using namespace kivo;
    core::playback::PlaybackSessionController session;
    ScriptedDecoder decoder;
    testing::time::ManualClock clock;
    const auto configuration = renderer_configuration();
    testing::render::FakeRenderer renderer{configuration, clock};
    core::playback::PlaybackRuntimeCoordinator runtime{
        session,
        decoder,
        renderer};

    RUNTIME_ASSERT(runtime.open(
        command(1, core::contract::CommandKind::OpenSource, 51),
        source(71, 52),
        open_request(configuration, 53)).succeeded());
    RUNTIME_ASSERT(runtime.execute(
        command(2, core::contract::CommandKind::Resume, 51)).succeeded());

    RuntimeTestCallGate open_gate;
    decoder.open_gate = &open_gate;
    core::playback::PlaybackRuntimeResult replacement{};
    std::thread replace_thread{[&] {
        replacement = runtime.open(
            command(3, core::contract::CommandKind::OpenSource, 61),
            source(72, 62),
            open_request(configuration, 63));
    }};
    open_gate.wait_until_entered();

    core::playback::PlaybackRuntimeResult pause{};
    std::thread pause_thread{[&] {
        pause = runtime.execute(
            command(4, core::contract::CommandKind::Pause, 51));
    }};
    open_gate.release();
    replace_thread.join();
    pause_thread.join();

    RUNTIME_ASSERT(replacement.succeeded());
    RUNTIME_ASSERT(!pause.succeeded());
    RUNTIME_ASSERT(
        pause.failure
        == core::playback::PlaybackRuntimeFailure::CommandRejected);
    const auto current = runtime.snapshot();
    RUNTIME_ASSERT(current.session.session_generation == 61);
    RUNTIME_ASSERT(
        current.session.state == core::contract::CoreState::Ready);
    RUNTIME_ASSERT(
        current.renderer.state
        == core::render::RenderLifecycleState::Open);
}

} // namespace playback_runtime_test
