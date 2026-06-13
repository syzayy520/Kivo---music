#pragma once

#include <cstdint>

namespace kivo::hardware_test::wasapi {

struct ToneOptions {
    double duration_seconds{3.0};
    double frequency_hz{440.0};
    double amplitude{0.18};
    uint32_t cycles{1};
    bool exclusive_mode{false};
};

[[nodiscard]] ToneOptions parse_tone_options(int argc, char** argv);

} // namespace kivo::hardware_test::wasapi
