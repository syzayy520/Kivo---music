#include "../fixture/wasapi_test_runner.hpp"

#include "platform/windows/wasapi/device/notification/default_render_endpoint_observer.hpp"

namespace {

using kivo::platform::windows::wasapi::detail::DefaultRenderEndpointObserver;

void default_console_change_publishes_once() {
    DefaultRenderEndpointObserver observer{L"current"};
    WASAPI_ASSERT(observer.OnDefaultDeviceChanged(
        eRender,
        eConsole,
        L"replacement") == S_OK);
    WASAPI_ASSERT(observer.change_count() == 1);
    WASAPI_ASSERT(observer.consume_change());
    WASAPI_ASSERT(!observer.consume_change());
}

void irrelevant_default_changes_are_ignored() {
    DefaultRenderEndpointObserver observer{L"current"};
    WASAPI_ASSERT(observer.OnDefaultDeviceChanged(
        eCapture,
        eConsole,
        L"replacement") == S_OK);
    WASAPI_ASSERT(observer.OnDefaultDeviceChanged(
        eRender,
        eMultimedia,
        L"replacement") == S_OK);
    WASAPI_ASSERT(observer.OnDefaultDeviceChanged(
        eRender,
        eConsole,
        L"current") == S_OK);
    WASAPI_ASSERT(observer.change_count() == 0);
    WASAPI_ASSERT(!observer.consume_change());
}

void current_endpoint_loss_publishes_change() {
    DefaultRenderEndpointObserver observer{L"current"};
    WASAPI_ASSERT(observer.OnDeviceStateChanged(
        L"current",
        DEVICE_STATE_DISABLED) == S_OK);
    WASAPI_ASSERT(observer.OnDeviceRemoved(L"current") == S_OK);
    WASAPI_ASSERT(observer.change_count() == 2);
    WASAPI_ASSERT(observer.consume_change());
}

} // namespace

void run_wasapi_endpoint_notification_tests(WasapiTestRunner& runner) {
    runner.run(
        "default_console_change_publishes_once",
        default_console_change_publishes_once);
    runner.run(
        "irrelevant_default_changes_are_ignored",
        irrelevant_default_changes_are_ignored);
    runner.run(
        "current_endpoint_loss_publishes_change",
        current_endpoint_loss_publishes_change);
}
