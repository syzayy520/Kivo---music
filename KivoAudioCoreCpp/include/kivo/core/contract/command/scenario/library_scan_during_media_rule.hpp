#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class LibraryScanDuringMediaRule : uint8_t {
    Unknown = 0,
    PauseMediaAndScan,
    BackgroundScan,
    RejectScan,
    QueueScan,
    CancelMedia
};

} // namespace kivo::core::contract