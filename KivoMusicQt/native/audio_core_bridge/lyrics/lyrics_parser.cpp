// =============================================================================
// Kivo Music Qt - lyrics_parser.cpp
// Lyrics: LRC text → timestamped lines map
// =============================================================================

#include "lyrics_parser.hpp"

#include <QRegularExpression>
#include <QStringList>

namespace kivo::qt::audio_bridge::lyrics {

QMap<qint64, QString> LyricsParser::parse(const QString& lrcContent) {
    QMap<qint64, QString> result;
    if (lrcContent.isEmpty()) return result;

    const QRegularExpression lineRx(R"(^\[(\d{2}):(\d{2})\.(\d{2,3})\](.*)$)");
    const QStringList lines = lrcContent.split('\n');

    for (const auto& line : lines) {
        const auto match = lineRx.match(line.trimmed());
        if (!match.hasMatch()) continue;

        const int minutes = match.captured(1).toInt();
        const int seconds = match.captured(2).toInt();
        const QString msStr = match.captured(3);
        const int millis = msStr.length() == 2 ? msStr.toInt() * 10 : msStr.toInt();
        const QString text = match.captured(4).trimmed();

        const qint64 timestamp = (minutes * 60 + seconds) * 1000LL + millis;
        if (!text.isEmpty()) {
            result.insert(timestamp, text);
        }
    }

    return result;
}

} // namespace kivo::qt::audio_bridge::lyrics
