#include "music_source_roots.h"

#include <QCoreApplication>
#include <QStandardPaths>

QList<MusicSourceRoot> MusicSourceRoots::builtIn() {
    QList<MusicSourceRoot> roots;

    const auto localMusic = QStandardPaths::writableLocation(
        QStandardPaths::MusicLocation);
    if (!localMusic.isEmpty()) {
        // Chinese-source label (kivo_en.ts maps it to "Local Music" for EN).
        roots.append({localMusic,
                      QCoreApplication::translate("MusicSourceRoots", "Local Music")});
    }

    return roots;
}
