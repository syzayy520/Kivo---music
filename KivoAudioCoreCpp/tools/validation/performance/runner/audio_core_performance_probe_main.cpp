#include <array>
#include <charconv>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string_view>

#include "../foundation/performance_probe_model.hpp"
#include "../measurement/decode_latency_measurement.hpp"
#include "../measurement/queue_timing_measurement.hpp"
#include "../reporting/performance_evidence_writer.hpp"

namespace {

[[nodiscard]] uint32_t parse_iterations(const char* value) {
    uint32_t result = 0;
    const std::string_view text{value};
    const auto parsed =
        std::from_chars(text.data(), text.data() + text.size(), result);
    if (parsed.ec != std::errc{}
        || parsed.ptr != text.data() + text.size()
        || result == 0
        || result > 100) {
        throw std::invalid_argument{"iterations must be between 1 and 100"};
    }
    return result;
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc != 3 && argc != 4) {
            std::cerr
                << "Usage: kivo_audio_core_performance_probe "
                   "<fixture-directory> <output-json> [iterations]\n";
            return 2;
        }

        const std::filesystem::path fixture_directory{argv[1]};
        const std::filesystem::path output_path{argv[2]};
        const auto iterations = argc == 4 ? parse_iterations(argv[3]) : 5;
        constexpr std::array<std::string_view, 7> fixture_names{
            "tone_pcm_s16le.wav",
            "tone.flac",
            "tone.mp3",
            "tone_aac.m4a",
            "tone_alac.m4a",
            "tone_vorbis.ogg",
            "tone_opus.ogg"
        };

        kivo::validation::performance::PerformanceProbeEvidence evidence{};
        evidence.iterations = iterations;
        evidence.fixtures.reserve(fixture_names.size());
        for (const auto fixture_name : fixture_names) {
            const auto fixture_path = fixture_directory / fixture_name;
            if (!std::filesystem::is_regular_file(fixture_path)) {
                throw std::runtime_error{"required performance fixture missing"};
            }
            std::cout << "Measuring " << fixture_name << "...\n";
            evidence.fixtures.push_back(
                kivo::validation::performance::measure_decode_latency(
                    fixture_path,
                    iterations));
        }
        std::cout << "Measuring queue occupancy and timing...\n";
        evidence.queue =
            kivo::validation::performance::measure_queue_timing();
        kivo::validation::performance::write_performance_evidence(
            output_path,
            evidence);
        std::cout << "Performance measurement completed.\n";
        return 0;
    } catch (const std::exception& exception) {
        std::cerr << "Performance measurement failed: "
                  << exception.what() << "\n";
        return 1;
    }
}
