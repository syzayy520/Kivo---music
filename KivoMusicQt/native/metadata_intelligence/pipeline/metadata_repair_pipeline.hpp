#pragma once

#include "../report/metadata_repair_report.hpp"

#include "../../sources/music/music_file_record.h"

#include <QList>

namespace kivo::metadata {

// =============================================================================
// MetadataRepairPipeline
// 职责：编排 detector -> corrector -> report 的只读 dry-run 流程
// 单文件单职责：接收 MusicFileRecord 列表，返回 MetadataRepairReport
// 禁止：写数据库、写源文件 tag、修改输入记录、调用 applyMusicRecords
// =============================================================================
class MetadataRepairPipeline {
public:
    // Dry-run analysis: detect and formalize candidates, then build a report.
    // The input records are never modified.
    static MetadataRepairReport run(const QList<MusicFileRecord>& records);
};

}  // namespace kivo::metadata