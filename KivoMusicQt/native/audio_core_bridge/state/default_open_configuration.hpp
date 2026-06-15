// =============================================================================
// Kivo Music Qt - default_open_configuration.hpp
// AudioCoreBridge: Default Host ABI open configuration
// =============================================================================

#pragma once

#include "../loader/audio_core_function_resolver.hpp"

namespace kivo::qt::audio_bridge::state {

[[nodiscard]] kivo_audio_open_configuration_v1 makeDefaultOpenConfiguration() noexcept;

} // namespace kivo::qt::audio_bridge::state
