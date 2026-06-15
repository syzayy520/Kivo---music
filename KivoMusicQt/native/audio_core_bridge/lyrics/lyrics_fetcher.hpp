// =============================================================================
// Kivo Music Qt - lyrics_fetcher.hpp
// Lyrics: Fetch lyrics from local .lrc file or online API
// =============================================================================

#pragma once

#include <QObject>
#include <QString>
#include <QMap>

namespace kivo::qt::audio_bridge::lyrics {

class LyricsFetcher : public QObject {
    Q_OBJECT

public:
    explicit LyricsFetcher(QObject* parent = nullptr);

    // Try local .lrc first, then online API
    // Returns empty string if lyrics unavailable
    Q_INVOKABLE QString fetchLyrics(const QString& audioFilePath);

signals:
    void lyricsReady(const QString& audioFilePath, const QMap<qint64, QString>& lyrics);
    void lyricsFailed(const QString& audioFilePath);

private:
    QString fetchLocal(const QString& audioFilePath) const;
    QString fetchOnline(const QString& artist, const QString& title) const;
    QString lrcFilePath(const QString& audioFilePath) const;
};

} // namespace kivo::qt::audio_bridge::lyrics
