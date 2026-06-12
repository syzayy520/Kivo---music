#pragma once

namespace hires_pcm_test {

void high_sample_rate_pcm_descriptors_are_valid();
void valid_bits_and_container_bits_remain_distinct();
void frame_size_uses_container_bits();
void pcm_sample_formats_remain_integer_or_float();
void unsupported_non_pcm_payload_is_not_modelled_as_pcm();

} // namespace hires_pcm_test
