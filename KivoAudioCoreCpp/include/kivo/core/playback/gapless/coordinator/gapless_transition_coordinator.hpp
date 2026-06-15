#pragma once

#include <memory>

#include "kivo/core/playback/gapless/coordinator/gapless_coordinator_result.hpp"
#include "kivo/core/playback/gapless/coordinator/gapless_coordinator_snapshot.hpp"
#include "kivo/core/playback/gapless/coordinator/gapless_prepare_result.hpp"
#include "kivo/core/playback/gapless/coordinator/gapless_track_prepare_request.hpp"
#include "kivo/core/render/queue/spsc_audio_block_queue.hpp"

namespace kivo::core::playback {

class GaplessTransitionCoordinator final {
public:
    [[nodiscard]] static std::unique_ptr<GaplessTransitionCoordinator> create(
        render::SpscAudioBlockQueue& queue,
        const GaplessTrackPrepareRequest& first_track) noexcept;

    ~GaplessTransitionCoordinator();

    GaplessTransitionCoordinator(
        const GaplessTransitionCoordinator&) = delete;
    GaplessTransitionCoordinator& operator=(
        const GaplessTransitionCoordinator&) = delete;

    [[nodiscard]] GaplessPrepareResult prepare_next(
        const GaplessTrackPrepareRequest& next_track) noexcept;
    [[nodiscard]] GaplessCoordinatorResult step() noexcept;
    [[nodiscard]] GaplessCoordinatorSnapshot snapshot() const noexcept;

private:
    class Impl;
    explicit GaplessTransitionCoordinator(
        std::unique_ptr<Impl> impl) noexcept;

    std::unique_ptr<Impl> impl_;
};

} // namespace kivo::core::playback
