// =============================================================================
// Kivo Music Qt - source_root_composer.cpp
// =============================================================================

#include "source_root_composer.hpp"

#include <QDir>
#include <QSet>

#include "../music/music_source_roots.h"

namespace kivo::qt::sources {

QString SourceRootComposer::normalizeNetworkPath(const QString& input) {
    QString value = input.trimmed();
    if (value.isEmpty()) {
        return {};
    }

    // Normalize separators FIRST so scheme/UNC detection is agnostic to slash
    // direction (handles "smb:\\host\share" and "\\host\share" alike).
    value.replace('\\', '/');

    // Strip an smb: scheme regardless of how many slashes follow it.
    if (value.startsWith(QStringLiteral("smb:"), Qt::CaseInsensitive)) {
        value = value.mid(4);  // drop "smb:", leaving "//host/share" or "/host/.."
    }

    // Count and strip any run of leading slashes.
    int slashes = 0;
    while (slashes < value.size() && value.at(slashes) == QLatin1Char('/')) {
        ++slashes;
    }
    QString rest = value.mid(slashes);
    if (rest.isEmpty()) {
        return {};
    }

    // Drop trailing slashes once, shared by both branches, so "X/" and "X" of the
    // SAME folder normalize identically and de-duplicate correctly.
    while (rest.size() > 1 && rest.endsWith(QLatin1Char('/'))) {
        rest.chop(1);
    }

    // A drive-letter local path (e.g. "D:/Music") is kept as a local path.
    if (slashes == 0 && rest.size() >= 2 && rest.at(1) == QLatin1Char(':')) {
        return rest;
    }

    // Everything else is treated as a UNC network share root.
    return QStringLiteral("//") + rest;
}

QString SourceRootComposer::deriveLabel(const QString& normalizedPath) {
    const QStringList parts =
        normalizedPath.split(QLatin1Char('/'), Qt::SkipEmptyParts);
    if (parts.isEmpty()) {
        return QStringLiteral("Network Folder");
    }
    return parts.last();
}

QList<MusicSourceRoot> SourceRootComposer::compose(
    const QList<UserSourceRoot>& userRoots) {
    QList<MusicSourceRoot> result;
    QSet<QString> seen;

    const auto addUnique = [&](const QString& path, const QString& label) {
        if (path.isEmpty()) {
            return;
        }
        const QString key = path.toLower();
        if (seen.contains(key)) {
            return;
        }
        seen.insert(key);
        result.append({path, label});
    };

    // Built-in Local Music — always first.
    for (const auto& builtin : MusicSourceRoots::builtIn()) {
        addUnique(builtin.path, builtin.label);
    }

    // User network roots — enabled only.
    for (const auto& user : userRoots) {
        if (!user.enabled) {
            continue;
        }
        addUnique(
            user.path,
            user.label.isEmpty() ? deriveLabel(user.path) : user.label);
    }

    return result;
}

} // namespace kivo::qt::sources
