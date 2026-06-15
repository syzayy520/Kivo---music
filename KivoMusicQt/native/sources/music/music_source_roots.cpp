#include "music_source_roots.h"

#include <QStandardPaths>

QList<MusicSourceRoot> MusicSourceRoots::defaults() {
    QList<MusicSourceRoot> roots;

    const auto localMusic = QStandardPaths::writableLocation(
        QStandardPaths::MusicLocation);
    if (!localMusic.isEmpty()) {
        roots.append({localMusic, "Local Music"});
    }

    const auto nasMusic = QStringLiteral("//192.168.5.1/")
        + QString::fromUtf8("\xE9\x9F\xB3\xE4\xB9\x90");
    roots.append({nasMusic, "NAS Music"});

    return roots;
}
