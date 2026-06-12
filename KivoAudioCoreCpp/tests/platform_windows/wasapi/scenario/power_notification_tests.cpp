#include "../fixture/wasapi_test_runner.hpp"

#include <Windows.h>

#include "platform/windows/wasapi/power/notification/audio_power_transition_observer.hpp"

namespace {

using kivo::platform::windows::wasapi::detail::
    AudioPowerTransitionObserver;

void suspend_and_resume_publish_bounded_transition() {
    AudioPowerTransitionObserver observer;
    WASAPI_ASSERT(observer.on_notification(PBT_APMSUSPEND) == ERROR_SUCCESS);
    WASAPI_ASSERT(
        observer.on_notification(PBT_APMRESUMEAUTOMATIC)
        == ERROR_SUCCESS);
    WASAPI_ASSERT(observer.transition_count() == 2);
    WASAPI_ASSERT(observer.consume_transition());
    WASAPI_ASSERT(!observer.consume_transition());
}

void unrelated_power_events_are_ignored() {
    AudioPowerTransitionObserver observer;
    WASAPI_ASSERT(observer.on_notification(0xffffffffu) == ERROR_SUCCESS);
    WASAPI_ASSERT(observer.transition_count() == 0);
    WASAPI_ASSERT(!observer.consume_transition());
}

} // namespace

void run_wasapi_power_notification_tests(WasapiTestRunner& runner) {
    runner.run(
        "suspend_and_resume_publish_bounded_transition",
        suspend_and_resume_publish_bounded_transition);
    runner.run(
        "unrelated_power_events_are_ignored",
        unrelated_power_events_are_ignored);
}
