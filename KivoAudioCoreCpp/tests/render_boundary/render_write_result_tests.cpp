#include "render_boundary_tests_main.hpp"

#include <chrono>

#include "kivo/core/render/write/render_write_result.hpp"

namespace {

using namespace kivo::core;

void full_write_transfers_all_frames() {
    const auto result = render::RenderWriteResult::Full(256);
    RENDER_ASSERT(result.disposition() == render::RenderWriteDisposition::Full);
    RENDER_ASSERT(result.accepted_frames() == 256);
    RENDER_ASSERT(result.unwritten_frames() == 0);
    RENDER_ASSERT(result.unwritten_ownership() == render::UnwrittenFrameOwnership::None);
}

void partial_write_retains_unwritten_frames_for_caller() {
    const auto result = render::RenderWriteResult::Partial(256, 96);
    RENDER_ASSERT(result.disposition() == render::RenderWriteDisposition::Partial);
    RENDER_ASSERT(result.accepted_frames() == 96);
    RENDER_ASSERT(result.unwritten_frames() == 160);
    RENDER_ASSERT(result.unwritten_ownership() == render::UnwrittenFrameOwnership::Caller);
    RENDER_ASSERT(result.failure() == render::RenderFailure::None);
}

void invalid_partial_write_is_rejected() {
    const auto result = render::RenderWriteResult::Partial(64, 64);
    RENDER_ASSERT(result.disposition() == render::RenderWriteDisposition::Rejected);
    RENDER_ASSERT(result.failure() == render::RenderFailure::InvalidBuffer);
    RENDER_ASSERT(result.accepted_frames() == 0);
}

void delayed_write_accepts_no_frames_and_reports_retry() {
    const auto result =
        render::RenderWriteResult::Delayed(128, std::chrono::microseconds{500});
    RENDER_ASSERT(result.disposition() == render::RenderWriteDisposition::Delayed);
    RENDER_ASSERT(result.accepted_frames() == 0);
    RENDER_ASSERT(result.unwritten_ownership() == render::UnwrittenFrameOwnership::Caller);
    RENDER_ASSERT(result.retry_after() == std::chrono::microseconds{500});
}

void zero_frame_delayed_write_is_rejected() {
    const auto result =
        render::RenderWriteResult::Delayed(0, std::chrono::microseconds{500});
    RENDER_ASSERT(result.disposition() == render::RenderWriteDisposition::Rejected);
    RENDER_ASSERT(result.failure() == render::RenderFailure::InvalidBuffer);
}

void rejected_and_failed_writes_preserve_failure_truth() {
    const auto rejected =
        render::RenderWriteResult::Rejected(32, render::RenderFailure::StaleSeekGeneration);
    RENDER_ASSERT(rejected.disposition() == render::RenderWriteDisposition::Rejected);
    RENDER_ASSERT(rejected.failure() == render::RenderFailure::StaleSeekGeneration);
    RENDER_ASSERT(rejected.unwritten_frames() == 32);

    const auto failed =
        render::RenderWriteResult::Failed(32, render::RenderFailure::DeviceLost);
    RENDER_ASSERT(failed.disposition() == render::RenderWriteDisposition::Failed);
    RENDER_ASSERT(failed.failure() == render::RenderFailure::DeviceLost);
    RENDER_ASSERT(failed.accepted_frames() == 0);
}

} // namespace

void run_render_write_result_tests(RenderBoundaryTestRunner& runner) {
    runner.run("full_write_transfers_all_frames", full_write_transfers_all_frames);
    runner.run(
        "partial_write_retains_unwritten_frames_for_caller",
        partial_write_retains_unwritten_frames_for_caller);
    runner.run("invalid_partial_write_is_rejected", invalid_partial_write_is_rejected);
    runner.run(
        "delayed_write_accepts_no_frames_and_reports_retry",
        delayed_write_accepts_no_frames_and_reports_retry);
    runner.run("zero_frame_delayed_write_is_rejected", zero_frame_delayed_write_is_rejected);
    runner.run(
        "rejected_and_failed_writes_preserve_failure_truth",
        rejected_and_failed_writes_preserve_failure_truth);
}
