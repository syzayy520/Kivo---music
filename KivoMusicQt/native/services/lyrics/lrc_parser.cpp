// =============================================================================
// Kivo Music Qt - lrc_parser.cpp
// =============================================================================

#include "lrc_parser.hpp"

#include <QRegularExpression>

namespace kivo::qt::lyrics {

QVector<LrcLine> LrcParser::parse(const QString& lrcContent) {
    QVector<LrcLine> lines;
    if (lrcContent.isEmpty()) {
        return lines;
    }

    // 匹配 [mm:ss.xx] 或 [mm:ss]  后面可能跟文本
    static const QRegularExpression linePattern(
        R"(\[(\d+):(\d+)(?:\.(\d+))?\](.*))",
        QRegularExpression::MultilineOption
    );

    const auto textLines = lrcContent.split('\n', Qt::SkipEmptyParts);
    for (const auto& line : textLines) {
        auto match = linePattern.match(line);
        if (!match.hasMatch()) {
            continue;  // 跳过非歌词行（元数据、注释等）
        }

        const auto minutes = match.captured(1).toInt();
        const auto seconds = match.captured(2).toInt();
        const auto centiseconds = match.captured(3).isEmpty()
            ? 0
            : match.captured(3).leftJustified(2, '0', true).toInt();
        const auto text = match.captured(4).trimmed();

        LrcLine lrcLine;
        lrcLine.timestampMs = (minutes * 60 + seconds) * 1000 + centiseconds * 10;
        lrcLine.text = text;
        lines.append(lrcLine);
    }

    // 按时间排序
    std::sort(lines.begin(), lines.end(), [](const LrcLine& a, const LrcLine& b) {
        return a.timestampMs < b.timestampMs;
    });

    return lines;
}

qint64 LrcParser::parseTimestamp(const QString& tag) {
    static const QRegularExpression tsPattern(R"(\[(\d+):(\d+)(?:\.(\d+))?\])");
    auto match = tsPattern.match(tag);
    if (!match.hasMatch()) {
        return -1;
    }

    const auto minutes = match.captured(1).toInt();
    const auto seconds = match.captured(2).toInt();
    const auto centiseconds = match.captured(3).isEmpty()
        ? 0
        : match.captured(3).leftJustified(2, '0', true).toInt();

    return (minutes * 60 + seconds) * 1000 + centiseconds * 10;
}

} // namespace kivo::qt::lyrics
