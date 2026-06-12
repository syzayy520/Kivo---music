#pragma once

#include "../fixture/decode_output_smoke_context.hpp"

namespace kivo::hardware_test::decode_output {

[[nodiscard]] int open_decode_output_smoke(DecodeOutputSmokeContext& context);
[[nodiscard]] int play_decode_output_smoke(DecodeOutputSmokeContext& context);
[[nodiscard]] int complete_decode_output_smoke(DecodeOutputSmokeContext& context);

} // namespace kivo::hardware_test::decode_output
