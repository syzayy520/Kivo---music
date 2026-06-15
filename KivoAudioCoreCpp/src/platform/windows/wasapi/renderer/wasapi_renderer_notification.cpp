#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

namespace kivo::platform::windows::wasapi::detail {

bool WasapiRendererState::detect_render_environment_change() noexcept {
    const auto endpoint_changed =
        endpoint_observer_ != nullptr
        && endpoint_observer_->consume_change();
    const auto power_changed = power_observer_.consume_transition();
    if (!endpoint_changed && !power_changed) {
        return false;
    }

    if (power_changed) {
        diagnostics_.power_transition_count =
            power_observer_.transition_count();
    }
    ++diagnostics_.device_invalidation_count;
    diagnostics_.reopen_required = true;
    diagnostics_.last_platform_code = power_changed
        ? AUDCLNT_E_RESOURCES_INVALIDATED
        : AUDCLNT_E_DEVICE_INVALIDATED;
    device_generation_.id =
        core::contract::GenerationId::next(device_generation_.id);
    diagnostics_.device_generation = device_generation_;
    snapshot_.generations.device = device_generation_;
    snapshot_.last_failure = core::render::RenderFailure::DeviceLost;
    snapshot_.state = core::render::RenderLifecycleState::Failed;
    publish_snapshot();
    return true;
}

} // namespace kivo::platform::windows::wasapi::detail
