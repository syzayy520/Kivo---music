#include "host/abi/validation/host_abi_validation.hpp"

namespace kivo::host::abi {

bool is_boolean(uint32_t value) noexcept {
    return value == 0u || value == 1u;
}

} // namespace kivo::host::abi
