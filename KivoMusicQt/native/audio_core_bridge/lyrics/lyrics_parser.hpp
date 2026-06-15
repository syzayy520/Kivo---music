// =============================================================================
// Kivo Music Qt - lyrics_parser.hpp
// Lyrics: LRC text → timestamped lines map
// =============================================================================

#pragma once

#include <QMap>
#include <QString>

namespace kivo::qt::audio_bridge::lyrics {

class LyricsParser {
public:
    // Parse LRC content into timestamp→text map
    // Returns empty map on failure
    static QMap<qint64, QString> parse(const QString& lrcContent);

private:
    static qint64 parseTimestamp(const QString& tag);
};

} // namespace kivo::qt::audio_bridge::lyrics
