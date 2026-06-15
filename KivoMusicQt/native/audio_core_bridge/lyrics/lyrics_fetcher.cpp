// =============================================================================
// Kivo Music Qt - lyrics_fetcher.cpp
// Lyrics: Fetch lyrics from local .lrc file or online API
// =============================================================================

#include "lyrics_fetcher.hpp"
#include "lyrics_parser.hpp"

#include <QFile>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

namespace kivo::qt::audio_bridge::lyrics {

LyricsFetcher::LyricsFetcher(QObject* parent) : QObject(parent) {}

QString LyricsFetcher::lrcFilePath(const QString& audioFilePath) const {
    QFileInfo info(audioFilePath);
    const QString dir = info.absolutePath();
    const QString base = info.completeBaseName();
    return dir + "/" + base + ".lrc";
}

QString LyricsFetcher::fetchLocal(const QString& audioFilePath) const {
    const QString lrcPath = lrcFilePath(audioFilePath);
    QFile file(lrcPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return {};
    return QString::fromUtf8(file.readAll());
}

QString LyricsFetcher::fetchOnline(const QString& artist, const QString& title) const {
    // Placeholder: online API not yet implemented
    // Future: integrate NetEase Cloud Music / QQ Music lyrics API
    Q_UNUSED(artist);
    Q_UNUSED(title);
    return {};
}

QString LyricsFetcher::fetchLyrics(const QString& audioFilePath) {
    if (audioFilePath.isEmpty()) return {};

    // Step 1: Try local .lrc file
    QString content = fetchLocal(audioFilePath);
    if (!content.isEmpty()) return content;

    // Step 2: Try online API (placeholder)
    QFileInfo info(audioFilePath);
    const QString fileName = info.completeBaseName();
    const QStringList parts = fileName.split(" - ");
    const QString artist = parts.size() >= 2 ? parts[0].trimmed() : QString();
    const QString title = parts.size() >= 2 ? parts[1].trimmed() : fileName;

    content = fetchOnline(artist, title);
    if (!content.isEmpty()) return content;

    emit lyricsFailed(audioFilePath);
    return {};
}

} // namespace kivo::qt::audio_bridge::lyrics
