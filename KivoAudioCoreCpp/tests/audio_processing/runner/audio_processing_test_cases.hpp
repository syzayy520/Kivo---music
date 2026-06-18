#pragma once

namespace audio_processing_test {

void default_bypass_is_byte_identical();
void unity_enabled_stages_are_bypassed();
void replay_gain_and_volume_are_independent();
void peak_prevention_limits_gain();
void integer_formats_apply_gain_and_saturate();
void floating_formats_apply_gain_and_saturate();
void triangular_dither_is_deterministic();
void bit_perfect_conflicts_are_rejected();
void invalid_policy_is_rejected();
void chain_set_volume_recomputes_gain_dither_and_bypass();
void chain_set_volume_refuses_under_bit_perfect();

} // namespace audio_processing_test
