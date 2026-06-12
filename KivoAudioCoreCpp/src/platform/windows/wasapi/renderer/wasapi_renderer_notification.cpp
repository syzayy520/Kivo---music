#include "platform/windows/wasapi/renderer/wasapi_renderer_state.hpp"

namespace kivo::platform::windows::wasapi::detail {

bool WasapiRendererState::detect_endpoint_change() noexcept {
    if (endpoint_observer_ == nullptr
        || !endpoint_observer_->consume_change()) {
        return false;
    }

    ++diagnostics_.device_invalidation_count;
    diagnostics_.reopen_required = true;
    diagnostics_.last_platform_code = AUDCLNT_E_DEVICE_INVALIDATED;
    device_generation_.id =
        core::contract::GenerationId::next(device_generation_.id);
    diagnostics_.device_generation = device_generation_;
    snapshot_.generations.device = device_generation_;
    snapshot_.last_failure = core::render::RenderFailure::DeviceLost;
    snapshot_.state = core::render::RenderLifecycleState::Failed;
    return true;
}

} // namespace kivo::platform::windows::wasapi::detail
