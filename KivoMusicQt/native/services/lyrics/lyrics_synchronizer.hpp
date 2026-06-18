// =============================================================================
// Kivo Music Qt - lyrics_synchronizer.hpp
// 职责：根据播放位置（毫秒）匹配当前/下一行歌词。单一职责：只同步，不解析。
// =============================================================================

#pragma once

#include "lrc_parser.hpp"

namespace kivo::qt::lyrics {

struct SyncResult {
    int currentIndex{-1};  // -1 = 无匹配行
    int nextIndex{-1};
};

class LyricsSynchronizer final {
public:
    explicit LyricsSynchronizer(const QVector<LrcLine>& lines);

    // 根据播放位置（毫秒）返回当前行和下一行索引。
    // 当前行 = timestampMs <= positionMs 的最后一行。
    SyncResult syncAt(qint64 positionMs) const;

    const QVector<LrcLine>& lines() const { return lines_; }
    bool isEmpty() const { return lines_.isEmpty(); }

private:
    QVector<LrcLine> lines_;
};

} // namespace kivo::qt::lyrics
