// =============================================================================
// Kivo Music Qt - lyrics_synchronizer.cpp
// =============================================================================

#include "lyrics_synchronizer.hpp"

#include <algorithm>

namespace kivo::qt::lyrics {

LyricsSynchronizer::LyricsSynchronizer(const QVector<LrcLine>& lines)
    : lines_(lines) {
}

SyncResult LyricsSynchronizer::syncAt(qint64 positionMs) const {
    SyncResult result;
    if (lines_.isEmpty() || positionMs < 0) {
        return result;
    }

    // 二分查找：找到 timestampMs <= positionMs 的最后一行
    auto it = std::upper_bound(
        lines_.begin(), lines_.end(), positionMs,
        [](qint64 pos, const LrcLine& line) {
            return pos < line.timestampMs;
        }
    );

    if (it == lines_.begin()) {
        // 播放位置在第一行之前 → 显示第一行作为"即将到来"
        result.nextIndex = 0;
        return result;
    }

    // 当前行 = it - 1
    result.currentIndex = static_cast<int>(std::distance(lines_.begin(), it - 1));

    // 下一行
    if (it != lines_.end()) {
        result.nextIndex = static_cast<int>(std::distance(lines_.begin(), it));
    }

    return result;
}

} // namespace kivo::qt::lyrics
