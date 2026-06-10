// =============================================================================
// KivoAudioCoreCpp — device_format_match.hpp
// Contract: Whether device format equals source format
// =============================================================================
//
// Confirms DeviceFormatEqualsSourceFormat truth.
// References P0-C-004 AudioFormatDescriptor, does not redefine it.
// Platform-neutral.
// =============================================================================

#pragma once

#include <cstdint>
#include <optional>

#include "../../format/descriptor/audio_format_descriptor.hpp"

namespace kivo::core::contract {

// =============================================================================
// DeviceFormatMatch — Device format vs source format match confirmation
// =============================================================================
struct DeviceFormatMatch {
    bool formats_equal = false;
    std::optional<AudioFormatDescriptor> source_format;
    std::optional<AudioFormatDescriptor> device_format;

    // --- Convenience ---
    [[nodiscard]] bool is_match() const noexcept {
        return formats_equal;
    }

    // --- Comparison ---
    [[nodiscard]] bool operator==(const DeviceFormatMatch&) const noexcept = default;
};

} // namespace kivo::core::contract
