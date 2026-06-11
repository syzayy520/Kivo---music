#include "wasapi_tests_main.hpp"

#include <Windows.h>
#include <audioclient.h>

#include "wasapi_error.hpp"

namespace {

using namespace kivo;

void success_maps_to_no_failure() {
    WASAPI_ASSERT(
        platform::windows::wasapi::detail::map_wasapi_error(S_OK)
        == core::render::RenderFailure::None);
}

void invalidation_errors_map_to_device_lost() {
    WASAPI_ASSERT(platform::windows::wasapi::detail::is_device_invalidation(
        AUDCLNT_E_DEVICE_INVALIDATED));
    WASAPI_ASSERT(
        platform::windows::wasapi::detail::map_wasapi_error(
            AUDCLNT_E_RESOURCES_INVALIDATED)
        == core::render::RenderFailure::DeviceLost);
    WASAPI_ASSERT(
        platform::windows::wasapi::detail::map_wasapi_error(
            AUDCLNT_E_SERVICE_NOT_RUNNING)
        == core::render::RenderFailure::DeviceLost);
}

void unsupported_format_maps_explicitly() {
    WASAPI_ASSERT(
        platform::windows::wasapi::detail::map_wasapi_error(
            AUDCLNT_E_UNSUPPORTED_FORMAT)
        == core::render::RenderFailure::UnsupportedFormat);
}

void buffer_protocol_errors_map_to_overrun() {
    WASAPI_ASSERT(
        platform::windows::wasapi::detail::map_wasapi_error(
            AUDCLNT_E_BUFFER_TOO_LARGE)
        == core::render::RenderFailure::Overrun);
    WASAPI_ASSERT(
        platform::windows::wasapi::detail::map_wasapi_error(
            AUDCLNT_E_OUT_OF_ORDER)
        == core::render::RenderFailure::Overrun);
}

void unknown_platform_error_maps_to_boundary_failure() {
    WASAPI_ASSERT(
        platform::windows::wasapi::detail::map_wasapi_error(E_FAIL)
        == core::render::RenderFailure::BoundaryFailure);
}

} // namespace

void run_wasapi_error_mapping_tests(WasapiTestRunner& runner) {
    runner.run("success_maps_to_no_failure", success_maps_to_no_failure);
    runner.run(
        "invalidation_errors_map_to_device_lost",
        invalidation_errors_map_to_device_lost);
    runner.run("unsupported_format_maps_explicitly", unsupported_format_maps_explicitly);
    runner.run(
        "buffer_protocol_errors_map_to_overrun",
        buffer_protocol_errors_map_to_overrun);
    runner.run(
        "unknown_platform_error_maps_to_boundary_failure",
        unknown_platform_error_maps_to_boundary_failure);
}
