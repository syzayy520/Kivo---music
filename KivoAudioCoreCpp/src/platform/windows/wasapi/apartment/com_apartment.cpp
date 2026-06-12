#include "platform/windows/wasapi/apartment/com_apartment.hpp"

#include <objbase.h>

namespace kivo::platform::windows::wasapi::detail {

ComApartment::~ComApartment() {
    uninitialize();
}

HRESULT ComApartment::initialize() noexcept {
    if (initialized_) {
        return is_owner_thread() ? S_OK : RPC_E_WRONG_THREAD;
    }

    const auto result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (result == RPC_E_CHANGED_MODE) {
        initialized_ = true;
        owns_uninitialize_ = false;
        owner_thread_id_ = GetCurrentThreadId();
        return S_OK;
    }
    if (FAILED(result)) {
        return result;
    }

    initialized_ = true;
    owns_uninitialize_ = true;
    owner_thread_id_ = GetCurrentThreadId();
    return S_OK;
}

void ComApartment::uninitialize() noexcept {
    if (!initialized_ || !is_owner_thread()) {
        return;
    }
    if (owns_uninitialize_) {
        CoUninitialize();
    }
    owner_thread_id_ = 0;
    initialized_ = false;
    owns_uninitialize_ = false;
}

bool ComApartment::is_initialized() const noexcept {
    return initialized_;
}

bool ComApartment::is_owner_thread() const noexcept {
    return initialized_ && owner_thread_id_ == GetCurrentThreadId();
}

} // namespace kivo::platform::windows::wasapi::detail
