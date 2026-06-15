#pragma once

#include "../fixture/tone_options.hpp"

namespace kivo::hardware_test::wasapi {

[[nodiscard]] int run_wasapi_tone(const ToneOptions& options);

} // namespace kivo::hardware_test::wasapi
