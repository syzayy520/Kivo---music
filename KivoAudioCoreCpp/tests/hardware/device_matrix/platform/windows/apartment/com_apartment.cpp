#include "com_apartment.hpp"

#include <objbase.h>

namespace device_matrix::windows {

ComApartment::ComApartment() noexcept
    : result_(CoInitializeEx(nullptr, COINIT_MULTITHREADED)),
      uninitialize_(SUCCEEDED(result_)) {}

ComApartment::~ComApartment() {
    if (uninitialize_) {
        CoUninitialize();
    }
}

HRESULT ComApartment::result() const noexcept {
    return result_;
}

} // namespace device_matrix::windows
