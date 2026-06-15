#pragma once

#include <Windows.h>

namespace device_matrix::windows {

class ComApartment final {
public:
    ComApartment() noexcept;
    ~ComApartment();

    ComApartment(const ComApartment&) = delete;
    ComApartment& operator=(const ComApartment&) = delete;

    [[nodiscard]] HRESULT result() const noexcept;

private:
    HRESULT result_{E_FAIL};
    bool uninitialize_{false};
};

} // namespace device_matrix::windows
