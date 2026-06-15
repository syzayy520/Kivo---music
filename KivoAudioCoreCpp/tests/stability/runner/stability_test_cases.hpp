#pragma once

struct StabilityTestContext;

void repeated_fake_renderer_lifecycle_is_bounded(
    const StabilityTestContext& context);
void repeated_ffmpeg_lifecycle_has_bounded_resource_growth(
    const StabilityTestContext& context);
