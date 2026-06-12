#include "../fake_renderer_test_fixture.hpp"
#include "../fake_renderer_tests_main.hpp"

namespace {

void device_loss_during_flush_is_terminal() {
    using namespace kivo;
    testing::time::ManualClock clock;
    const auto configuration = fake_renderer_test::configuration();
    testing::render::FakeRenderer renderer{configuration, clock};

    FAKE_ASSERT(renderer.open(
        fake_renderer_test::open_request(configuration)).is_accepted());
    FAKE_ASSERT(renderer.start().is_success());
    FAKE_ASSERT(renderer.schedule_fault({
        testing::render::RenderFaultKind::DeviceLost,
        testing::render::RenderFaultTrigger::NextFlush
    }));

    const auto result = renderer.flush({
        fake_renderer_test::next_flush(
            renderer.snapshot().generations)});
    FAKE_ASSERT(!result.is_success());
    FAKE_ASSERT(
        result.failure() == core::render::RenderFailure::DeviceLost);
    FAKE_ASSERT(
        renderer.snapshot().state
        == core::render::RenderLifecycleState::Failed);
}

} // namespace

void run_fake_renderer_device_lifecycle_fault_tests(
    FakeRendererTestRunner& runner) {
    runner.run(
        "device_loss_during_flush_is_terminal",
        device_loss_during_flush_is_terminal);
}
