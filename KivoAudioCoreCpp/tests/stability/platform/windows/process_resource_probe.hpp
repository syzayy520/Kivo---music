#pragma once

#include <Windows.h>
#include <Psapi.h>

#include <cstdint>
#include <optional>

namespace stability_test {

struct ProcessResourceSnapshot {
    uint32_t handle_count{0};
    uint64_t private_bytes{0};
};

[[nodiscard]] inline std::optional<ProcessResourceSnapshot>
capture_process_resources() noexcept {
    DWORD handle_count = 0;
    if (GetProcessHandleCount(GetCurrentProcess(), &handle_count) == FALSE) {
        return std::nullopt;
    }

    PROCESS_MEMORY_COUNTERS_EX counters{};
    counters.cb = sizeof(counters);
    if (GetProcessMemoryInfo(
            GetCurrentProcess(),
            reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&counters),
            sizeof(counters)) == FALSE) {
        return std::nullopt;
    }
    return ProcessResourceSnapshot{
        handle_count,
        static_cast<uint64_t>(counters.PrivateUsage)
    };
}

[[nodiscard]] constexpr uint64_t positive_growth(
    uint64_t before,
    uint64_t after) noexcept {
    return after > before ? after - before : 0;
}

} // namespace stability_test
