#include "../contract_tests_main.h"

#include "kivo/core/contract/buffer/lifetime/buffer_lifetime_proof.hpp"
#include "kivo/core/contract/realtime/buffer/buffer_lifetime_rule.hpp"
#include "kivo/core/contract/format/descriptor/audio_format_descriptor.hpp"

namespace {

void buffer_lifetime_headers_are_composable() {
    auto general = kivo::core::contract::BufferLifetimeProof::Scoped;
    auto realtime = kivo::core::contract::RealtimeBufferLifetimeProof::MustBeProvableAcrossBoundary;

    ASSERT(general == kivo::core::contract::BufferLifetimeProof::Scoped);
    ASSERT(realtime == kivo::core::contract::RealtimeBufferLifetimeProof::MustBeProvableAcrossBoundary);
}

void format_descriptor_header_is_composable() {
    kivo::core::contract::AudioFormatDescriptor descriptor{};
    descriptor.sample_format = kivo::core::contract::SampleFormat::Int24;
    descriptor.channel_layout = kivo::core::contract::ChannelLayout::Stereo;
    descriptor.sample_rate = 48000;
    descriptor.bits_per_sample = 24;

    ASSERT(descriptor.is_valid());
    ASSERT(descriptor.bytes_per_frame() == 8);
}

} // namespace

void run_public_header_composability_tests(ContractTestRunner& runner) {
    runner.run("buffer_lifetime_headers_are_composable", buffer_lifetime_headers_are_composable);
    runner.run("format_descriptor_header_is_composable", format_descriptor_header_is_composable);
}
