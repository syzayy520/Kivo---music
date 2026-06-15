#pragma once

#include "decode_boundary_format_factory.hpp"

#include "kivo/core/decode/media/media_probe.hpp"

namespace kivo::decode_boundary::test {

[[nodiscard]] constexpr core::decode::MediaProbe valid_probe() noexcept {
    core::decode::MediaProbe probe{};
    probe.source_identity = {42};
    probe.codec = core::decode::AudioCodec::Flac;
    probe.container = core::decode::MediaContainer::Flac;
    probe.native_format = native_format();
    probe.output_format = output_format();
    probe.conversion = {true, true, false, true};
    probe.conversion_snapshot.conversion = probe.conversion;
    probe.resample = {
        true,
        core::contract::ResampleReason::DeviceMismatch,
        44100,
        48000
    };
    probe.duration_frames = 441000;
    probe.duration_known = true;
    probe.seekable = true;
    return probe;
}

} // namespace kivo::decode_boundary::test
