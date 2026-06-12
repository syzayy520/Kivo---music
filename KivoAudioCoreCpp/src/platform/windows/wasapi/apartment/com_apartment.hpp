#pragma once

#include <Windows.h>

namespace kivo::platform::windows::wasapi::detail {

class ComApartment {
public:
    ComApartment() noexcept = default;
    ~ComApartment();

    ComApartment(const ComApartment&) = delete;
    ComApartment& operator=(const ComApartment&) = delete;

    [[nodiscard]] HRESULT initialize() noexcept;
    void uninitialize() noexcept;
    [[nodiscard]] bool is_initialized() const noexcept;
    [[nodiscard]] bool is_owner_thread() const noexcept;

private:
    DWORD owner_thread_id_{0};
    bool initialized_{false};
    bool owns_uninitialize_{false};
};

} // namespace kivo::platform::windows::wasapi::detail
