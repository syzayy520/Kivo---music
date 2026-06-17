#pragma once

#include "../repair/id3_field_swap/field_swap_candidate.hpp"

#include <QList>

namespace kivo::metadata {

// =============================================================================
// FieldSwapCorrector
// 职责：对 detector 生成的候选建议做最终安全性校验，只生成/修正建议，不写入
// 单一职责：策略层安全守门，确保建议不会引入 Unknown Artist / 空 artist / placeholder
// =============================================================================

class FieldSwapCorrector {
public:
    // Re-validate a candidate. If the suggested swap would introduce an invalid
    // artist or title, the issue is downgraded to InvalidMetadata and the original
    // values are restored in the suggestion fields.
    static FieldSwapCandidate formalize(const FieldSwapCandidate& candidate);

    // Convenience: formalize a batch of candidates in order.
    static QList<FieldSwapCandidate> formalizeBatch(const QList<FieldSwapCandidate>& candidates);
};

}  // namespace kivo::metadata
