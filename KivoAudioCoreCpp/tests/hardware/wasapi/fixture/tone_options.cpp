#include "tone_options.hpp"

#include <algorithm>
#include <cstdlib>
#include <string_view>

namespace kivo::hardware_test::wasapi {

ToneOptions parse_tone_options(int argc, char** argv) {
    ToneOptions options{};
    for (int index = 1; index + 1 < argc; index += 2) {
        const std::string_view name{argv[index]};
        if (name == "--duration") {
            options.duration_seconds = std::strtod(argv[index + 1], nullptr);
        } else if (name == "--frequency") {
            options.frequency_hz = std::strtod(argv[index + 1], nullptr);
        } else if (name == "--amplitude") {
            options.amplitude = std::strtod(argv[index + 1], nullptr);
        } else if (name == "--cycles") {
            options.cycles = static_cast<uint32_t>(
                std::strtoul(argv[index + 1], nullptr, 10));
        }
    }
    options.duration_seconds = std::clamp(options.duration_seconds, 0.1, 3600.0);
    options.frequency_hz = std::clamp(options.frequency_hz, 20.0, 20000.0);
    options.amplitude = std::clamp(options.amplitude, 0.0, 0.5);
    options.cycles = std::clamp<uint32_t>(options.cycles, 1, 20);
    return options;
}

} // namespace kivo::hardware_test::wasapi
