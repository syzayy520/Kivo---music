#pragma once

#include <memory>

#include "kivo/core/contract/sample_position.hpp"
#include "kivo/core/decode/generation/decode_generation.hpp"
#include "kivo/core/decode/media/media_probe.hpp"
#include "kivo/core/decode/request/decode_open_request.hpp"
#include "kivo/core/decode/result/decode_control_result.hpp"
#include "kivo/core/decode/result/decode_open_result.hpp"
#include "kivo/core/decode/result/decode_step_result.hpp"
#include "kivo/core/decode/source/byte_source_boundary.hpp"

namespace kivo::core::decode {

class AudioDecodeBoundary {
public:
    virtual ~AudioDecodeBoundary() = default;

    [[nodiscard]] virtual DecodeOpenResult open(
        std::unique_ptr<ByteSourceBoundary> source,
        const DecodeOpenRequest& request) noexcept = 0;
    [[nodiscard]] virtual DecodeStepResult decode_next() noexcept = 0;
    [[nodiscard]] virtual DecodeControlResult seek(
        contract::SamplePosition target_frame,
        DecodeGeneration generation) noexcept = 0;
    [[nodiscard]] virtual DecodeControlResult flush(
        DecodeGeneration generation) noexcept = 0;
    [[nodiscard]] virtual DecodeControlResult close() noexcept = 0;
    [[nodiscard]] virtual MediaProbe probe() const noexcept = 0;
};

} // namespace kivo::core::decode
