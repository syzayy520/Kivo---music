#include "../fixture/decode_output_smoke_context.hpp"
#include "../scenario/decode_output_smoke_steps.hpp"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: kivo_ffmpeg_wasapi_playback_smoke <audio-file>\n";
        return 1;
    }

    kivo::hardware_test::decode_output::DecodeOutputSmokeContext context{
        std::filesystem::path{argv[1]}};
    auto result =
        kivo::hardware_test::decode_output::open_decode_output_smoke(context);
    if (result == 0) {
        result =
            kivo::hardware_test::decode_output::play_decode_output_smoke(context);
    }
    if (result == 0) {
        result =
            kivo::hardware_test::decode_output::complete_decode_output_smoke(context);
    }
    return result;
}
