#include "music_file_scanner.h"

#include "audio_file_type.h"
#include "music_cover_finder.h"
#include "music_filename_parser.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QHash>
#include <QtGlobal>

namespace {

QList<MusicFileRecord> scanRoot(const MusicSourceRoot& root, int maxFiles) {
    QList<MusicFileRecord> records;
    QHash<QString, QString> coverByDirectory;
    if (!QDir(root.path).exists()) {
        return records;
    }

    QDirIterator iterator(
        root.path,
        QDir::Files | QDir::Readable,
        QDirIterator::Subdirectories);

    while (iterator.hasNext() && records.size() < maxFiles) {
        const QFileInfo info(iterator.next());
        if (!AudioFileType::isSupported(info.suffix())) {
            continue;
        }

        const auto parsed = MusicFilenameParser::parse(info.completeBaseName());
        const auto directoryPath = info.dir().absolutePath();
        if (!coverByDirectory.contains(directoryPath)) {
            coverByDirectory.insert(
                directoryPath,
                MusicCoverFinder::findFor(info));
        }
        records.append({
            info.absoluteFilePath(),
            parsed.title,
            parsed.artist,
            info.dir().dirName(),
            root.label,
            AudioFileType::labelFor(info.suffix()),
            coverByDirectory.value(directoryPath),
            static_cast<int>(qHash(info.absoluteFilePath()) % 16)
        });
    }

    return records;
}

} // namespace

QList<MusicFileRecord> MusicFileScanner::scan(
    const QList<MusicSourceRoot>& roots,
    int maxFiles) {
    QList<MusicFileRecord> records;
    if (maxFiles <= 0) {
        return records;
    }

    const auto sourceBudget = qMax(1, maxFiles / qMax(1, roots.size()));
    QList<QList<MusicFileRecord>> buckets;
    for (const auto& root : roots) {
        const auto bucket = scanRoot(root, sourceBudget);
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
