#include "../fixture/tone_options.hpp"
#include "../scenario/wasapi_tone_playback.hpp"

int main(int argc, char** argv) {
    return kivo::hardware_test::wasapi::run_wasapi_tone(
        kivo::hardware_test::wasapi::parse_tone_options(argc, argv));
}
