#include "kivo/testing/render/renderer/fake_renderer.hpp"

namespace kivo::testing::render {

bool FakeRenderer::schedule_fault(const ScheduledRenderFault& fault) noexcept {
    if (!fault.is_valid()) {
        return false;
    }
    for (auto& slot : faults_) {
        if (!slot.active) {
            slot.fault = fault;
            slot.active = true;
            record(RenderObservationOperation::FaultScheduled);
            return true;
        }
    }
    return false;
}

std::optional<ScheduledRenderFault> FakeRenderer::take_write_fault(
    const core::render::AudioBlockView& block,
    uint64_t write_ordinal) noexcept {
    for (auto& slot : faults_) {
        if (!slot.active) {
            continue;
        }

        const auto& fault = slot.fault;
        const auto matches =
            fault.trigger == RenderFaultTrigger::NextWrite
            || (fault.trigger == RenderFaultTrigger::WriteCount
                && write_ordinal >= fault.trigger_value)
            || (fault.trigger == RenderFaultTrigger::FramePositionAtOrAfter
                && block.frame_offset >= fault.trigger_value)
            || (fault.trigger == RenderFaultTrigger::StreamGeneration
                && block.generations.stream.value() == fault.trigger_value);
        if (matches) {
            slot.active = false;
            return fault;
        }
    }
    return std::nullopt;
}

bool FakeRenderer::take_fault(
    RenderFaultKind kind,
    RenderFaultTrigger trigger) noexcept {
    for (auto& slot : faults_) {
        if (slot.active && slot.fault.kind == kind && slot.fault.trigger == trigger) {
            slot.active = false;
            return true;
        }
    }
    return false;
}

} // namespace kivo::testing::render
