#pragma once

#include <cstddef>
#include <cstdint>

namespace kivo::host::abi {

template <typename Structure>
[[nodiscard]] bool valid_input_structure(
    const Structure* structure) noexcept {
    return structure != nullptr
        && structure->struct_size >= sizeof(Structure)
        && structure->struct_version == 1u;
}

template <typename Structure>
[[nodiscard]] bool valid_output_structure(
    const Structure* structure) noexcept {
    return valid_input_structure(structure);
}

[[nodiscard]] bool is_boolean(uint32_t value) noexcept;

} // namespace kivo::host::abi
