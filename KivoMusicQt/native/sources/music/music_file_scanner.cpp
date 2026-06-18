#include "music_file_scanner.h"

#include "audio_file_type.h"
#include "embedded_cover_extractor.h"
#include "music_cover_finder.h"
#include "music_filename_parser.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QHash>
#include <QStandardPaths>
#include <QTcpSocket>
#include <QtGlobal>

namespace {

constexpr int kNetworkProbeTimeoutMs = 1200;
constexpr int kSmbPort = 445;

[[nodiscard]] bool isNetworkRoot(const QString& path) {
    return path.startsWith(QStringLiteral("//"))
        || path.startsWith(QStringLiteral("\\\\"));
}

// Host component of a UNC path (//host/share or \\host\share).
[[nodiscard]] QString networkHost(const QString& path) {
    QString normalized = path;
    normalized.replace('\\', '/');
    const auto parts = normalized.mid(2).split('/', Qt::SkipEmptyParts);
    return parts.isEmpty() ? QString() : parts.first();
}

// Bounded reachability check: a down/wrong NAS would otherwise block QDir on a
// multi-second SMB/filesystem timeout (and hang shutdown via the worker join).
[[nodiscard]] bool networkHostReachable(const QString& host) {
    if (host.isEmpty()) {
        return false;
    }
    QTcpSocket socket;
    socket.connectToHost(host, kSmbPort);
    const bool up = socket.waitForConnected(kNetworkProbeTimeoutMs);
    socket.abort();
    return up;
}

[[nodiscard]] bool isCancelled(const std::atomic<bool>* cancelled) {
    return cancelled != nullptr && cancelled->load(std::memory_order_relaxed);
}

QList<MusicFileRecord> scanRoot(
    const MusicSourceRoot& root,
    int maxFiles,
    const std::atomic<bool>* cancelled) {
    QList<MusicFileRecord> records;
    QHash<QString, QString> coverByDirectory;
    // Skip an unreachable network share quickly rather than blocking on it.
    if (isNetworkRoot(root.path) && !networkHostReachable(networkHost(root.path))) {
        return records;
    }
    if (isCancelled(cancelled) || !QDir(root.path).exists()) {
        return records;
    }

    QDirIterator iterator(
        root.path,
        QDir::Files | QDir::Readable,
        QDirIterator::Subdirectories);

    while (iterator.hasNext() && records.size() < maxFiles) {
        if (isCancelled(cancelled)) {
            break;
        }
        const QFileInfo info(iterator.next());
        if (!AudioFileType::isSupported(info.suffix())) {
            continue;
        }

        const auto parsed = MusicFilenameParser::parse(info.completeBaseName());
        const auto directoryPath = info.dir().absolutePath();
        // A shared folder image (cover.jpg / Artwork/) is cached per directory…
        if (!coverByDirectory.contains(directoryPath)) {
            coverByDirectory.insert(
                directoryPath,
                MusicCoverFinder::findFor(info));
        }
        // …but when the folder has none, fall back to THIS file's embedded art
        // (per-file: a folder of singles each carries its own cover).
        QString cover = coverByDirectory.value(directoryPath);
        if (cover.isEmpty()) {
            static const QString coverCacheDir =
                QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                + QStringLiteral("/covers");
            cover = EmbeddedCoverExtractor::extractTo(info, coverCacheDir);
        }
        records.append({
            info.absoluteFilePath(),
            parsed.title,
            parsed.artist,
            MusicFilenameParser::cleanDisplayField(info.dir().dirName()),
            root.label,
            AudioFileType::labelFor(info.suffix()),
            cover,
            static_cast<int>(qHash(info.absoluteFilePath()) % 16)
        });
    }

    return records;
}

} // namespace

QList<MusicFileRecord> MusicFileScanner::scan(
    const QList<MusicSourceRoot>& roots,
    int maxFiles,
    const std::atomic<bool>* cancelled) {
    QList<MusicFileRecord> records;
    if (maxFiles <= 0) {
        return records;
    }

    const auto sourceBudget = qMax(1, maxFiles / qMax(1, roots.size()));
    QList<QList<MusicFileRecord>> buckets;
    for (const auto& root : roots) {
        if (isCancelled(cancelled)) {
            break;
        }
        const auto bucket = scanRoot(root, sourceBudget, cancelled);
        if (!bucket.isEmpty()) {
            buckets.append(bucket);
        }
    }

    for (auto row = 0; records.size() < maxFiles; ++row) {
        auto appended = false;
        for (const auto& bucket : buckets) {
            if (row < bucket.size()) {
                records.append(bucket.at(row));
                appended = true;
                if (records.size() >= maxFiles) {
                    break;
                }
            }
        }
        if (!appended) {
            break;
        }
    }
    return records;
}
