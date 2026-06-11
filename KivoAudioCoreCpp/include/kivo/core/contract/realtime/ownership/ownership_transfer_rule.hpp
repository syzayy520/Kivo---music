#pragma once

#include <cstdint>

namespace kivo::core::contract {

enum class OwnershipTransfer : uint8_t {
    NotSpecified = 0,
    ExplicitTransferRequired,
    ImplicitTransferAllowed,
    TransferForbidden
};

} // namespace kivo::core::contract