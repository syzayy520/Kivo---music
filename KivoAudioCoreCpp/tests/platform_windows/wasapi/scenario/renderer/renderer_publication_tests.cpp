#include "../../fixture/wasapi_test_runner.hpp"

#include "platform/windows/wasapi/renderer/publication/wasapi_renderer_publication.hpp"

namespace {

using namespace kivo;

[[nodiscard]] core::contract::RenderFormat stereo_format() noexcept {
    return {{
        core::contract::SampleFormat::Float32,
        core::contract::ChannelLayout::Stereo,
        core::contract::FrameLayout::Interleaved,
        core::contract::kStereoMask,
        48000,
        32,
        32
    }};
}

[[nodiscard]] core::render::RenderGenerationSet generations() noexcept {
    return {
        {core::contract::GenerationId::from_value(2)},
        {core::contract::GenerationId::from_value(3)},
        {core::contract::GenerationId::from_value(4)},
        {core::contract::GenerationId::from_value(5)}
    };
}

void publication_round_trips_control_snapshot() {
    platform::windows::wasapi::detail::WasapiRendererPublication publication;
    const core::render::RenderSnapshot snapshot{
        core::render::RenderLifecycleState::Started,
        stereo_format(),
        512,
        64,
        128,
        generations(),
        core::render::RenderFailure::None
    };

    publication.publish(snapshot);

    WASAPI_ASSERT(publication.read() == snapshot);
}

void realtime_progress_updates_published_snapshot_only() {
    platform::windows::wasapi::detail::WasapiRendererPublication publication;
    core::render::RenderSnapshot snapshot{};
    snapshot.state = core::render::RenderLifecycleState::Started;
    snapshot.accepted_format = stereo_format();
    snapshot.buffer_capacity_frames = 512;
    publication.publish(snapshot);

    publication.publish_buffered_frames(96);
    publication.add_accepted_frames(24);
    publication.publish_failure(core::render::RenderFailure::Underrun);

    const auto current = publication.read();
    WASAPI_ASSERT(current.state == core::render::RenderLifecycleState::Started);
    WASAPI_ASSERT(current.buffered_frames == 96);
    WASAPI_ASSERT(current.total_accepted_frames == 24);
    WASAPI_ASSERT(current.last_failure == core::render::RenderFailure::Underrun);
}

void realtime_failure_publishes_failed_state() {
    platform::windows::wasapi::detail::WasapiRendererPublication publication;
    core::render::RenderSnapshot snapshot{};
    snapshot.state = core::render::RenderLifecycleState::Started;
    snapshot.accepted_format = stereo_format();
    snapshot.buffer_capacity_frames = 512;
    publication.publish(snapshot);

    publication.publish_failed_state(core::render::RenderFailure::DeviceLost);

    const auto current = publication.read();
    WASAPI_ASSERT(current.state == core::render::RenderLifecycleState::Failed);
    WASAPI_ASSERT(current.last_failure == core::render::RenderFailure::DeviceLost);
}

void control_snapshot_recovers_realtime_progress() {
    platform::windows::wasapi::detail::WasapiRendererPublication publication;
    core::render::RenderSnapshot snapshot{};
    snapshot.state = core::render::RenderLifecycleState::Started;
    snapshot.accepted_format = stereo_format();
    snapshot.buffer_capacity_frames = 512;
    publication.publish(snapshot);
    publication.publish_buffered_frames(128);
    publication.add_accepted_frames(64);
    publication.publish_failure(core::render::RenderFailure::Underrun);

    core::render::RenderSnapshot control_snapshot{};
    control_snapshot.state = core::render::RenderLifecycleState::Started;
    control_snapshot.accepted_format = stereo_format();
    control_snapshot.buffer_capacity_frames = 512;
    publication.sync_into(control_snapshot);

    WASAPI_ASSERT(control_snapshot.state
        == core::render::RenderLifecycleState::Started);
    WASAPI_ASSERT(control_snapshot.buffered_frames == 128);
    WASAPI_ASSERT(control_snapshot.total_accepted_frames == 64);
    WASAPI_ASSERT(control_snapshot.last_failure
        == core::render::RenderFailure::Underrun);
}

} // namespace

void run_wasapi_renderer_publication_tests(WasapiTestRunner& runner) {
    runner.run(
        "publication_round_trips_control_snapshot",
        publication_round_trips_control_snapshot);
    runner.run(
        "realtime_progress_updates_published_snapshot_only",
        realtime_progress_updates_published_snapshot_only);
    runner.run(
        "realtime_failure_publishes_failed_state",
        realtime_failure_publishes_failed_state);
    runner.run(
        "control_snapshot_recovers_realtime_progress",
        control_snapshot_recovers_realtime_progress);
}
