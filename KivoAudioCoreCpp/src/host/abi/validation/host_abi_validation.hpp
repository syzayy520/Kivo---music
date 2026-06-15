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

template <typename Structure>
[[nodiscard]] bool valid_output_structure_prefix(
    const Structure* structure,
    size_t minimum_size) noexcept {
    return structure != nullptr
        && structure->struct_size >= minimum_size
        && structure->struct_version == 1u;
}

[[nodiscard]] bool is_boolean(uint32_t value) noexcept;

} // namespace kivo::host::abi
