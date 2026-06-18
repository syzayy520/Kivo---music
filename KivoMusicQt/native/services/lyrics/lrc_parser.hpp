// =============================================================================
// Kivo Music Qt - lrc_parser.hpp
// 职责：解析 .lrc 歌词文件为时间戳+文本结构体列表。单一职责：只解析，不同步。
// 格式：[mm:ss.xx]歌词文本  或  [mm:ss]歌词文本
// =============================================================================

#pragma once

#include <QString>
#include <QVector>

namespace kivo::qt::lyrics {

struct LrcLine {
    qint64 timestampMs{0};  // 毫秒时间戳
    QString text;           // 歌词文本（去除时间标签）
};

class LrcParser final {
public:
    // 解析 .lrc 文件内容，返回按时间排序的歌词行列表。
    // 空文件/无效格式 → 空列表。多行同时间戳保留所有行。
    static QVector<LrcLine> parse(const QString& lrcContent);

private:
    // [mm:ss.xx] 或 [mm:ss] → 毫秒。失败返回 -1。
    static qint64 parseTimestamp(const QString& tag);
};

} // namespace kivo::qt::lyrics
