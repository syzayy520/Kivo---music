#pragma once

#include "../repair/id3_field_swap/field_swap_candidate.hpp"
#include "../../sources/music/music_file_record.h"

#include <QList>

namespace kivo::metadata {

// =============================================================================
// FieldSwapDetector
// 职责：检测 MusicFileRecord 中 title 与 artist 字段是否被 ID3 标签工具反填
// 单一职责：只生成 FieldSwapCandidate，不修改记录、不写库、不写源文件
// =============================================================================

class FieldSwapDetector {
public:
    // Single-record detection (no batch context). Always safe/dry-run.
    static FieldSwapCandidate detect(const MusicFileRecord& record);

    // Batch detection with album/folder-level evidence aggregation.
    // Every returned candidate contains albumTrackCount and swapCandidatesInAlbum.
    static QList<FieldSwapCandidate> detectBatch(const QList<MusicFileRecord>& records);
};

}  // namespace kivo::metadata
