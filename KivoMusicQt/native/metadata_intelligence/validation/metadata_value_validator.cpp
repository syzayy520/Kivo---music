#include "metadata_value_validator.hpp"

#include <QString>
#include <QStringList>

namespace kivo::metadata {

namespace {

QStringList englishPlaceholderPatterns() {
    return {
        QStringLiteral("unknown"),
        QStringLiteral("unknown artist"),
        QStringLiteral("unknown album"),
        QStringLiteral("unknown title"),
        QStringLiteral("unknown track"),
        QStringLiteral("unknown performer"),
        QStringLiteral("unknown composer"),
        QStringLiteral("unknown singer"),
        QStringLiteral("unknown musician"),
        QStringLiteral("unknown band"),
        QStringLiteral("artista desconocido"),
        QStringLiteral("artiste inconnu"),
        QStringLiteral("unbekannter interpret"),
        QStringLiteral("unbekannter kuenstler"),
        QStringLiteral(" interprete sconosciuto"),
    };
}

QStringList chinesePlaceholderPatterns() {
    return {
        QStringLiteral("未知"),
        QStringLiteral("未知艺人"),
        QStringLiteral("未知艺术家"),
        QStringLiteral("未知歌手"),
        QStringLiteral("未知音乐人"),
        QStringLiteral("未知专辑"),
        QStringLiteral("未知歌曲"),
        QStringLiteral("未知标题"),
        QStringLiteral("未知乐队"),
    };
}

}  // namespace

MetadataValueValidator::PlaceholderType MetadataValueValidator::getPlaceholderType(const QString& value) {
    if (value.isEmpty()) {
        return PlaceholderType::Empty;
    }

    const QString trimmed = value.trimmed();
    if (trimmed.isEmpty()) {
        return PlaceholderType::Empty;
    }

    const QString lower = trimmed.toLower();

    for (const auto& pattern : chinesePlaceholderPatterns()) {
        if (lower == pattern) {
            return PlaceholderType::UnknownChinese;
        }
    }

    for (const auto& pattern : englishPlaceholderPatterns()) {
        if (lower == pattern) {
            return PlaceholderType::UnknownEnglish;
        }
    }

    static const QStringList genericTokens = {
        QStringLiteral("unknown"),
        QStringLiteral("未知"),
    };
    for (const auto& token : genericTokens) {
        if (lower == token) {
            return PlaceholderType::GenericUnknown;
        }
    }

    return PlaceholderType::NotPlaceholder;
}

bool MetadataValueValidator::isPlaceholder(const QString& value) {
    return getPlaceholderType(value) != PlaceholderType::NotPlaceholder;
}

bool MetadataValueValidator::isValidArtist(const QString& artist) {
    return !isPlaceholder(artist);
}

bool MetadataValueValidator::isValidTitle(const QString& title) {
    return !isPlaceholder(title);
}

bool MetadataValueValidator::isValidAlbum(const QString& album) {
    // An empty album is acceptable; a non-empty album must not be a placeholder.
    return album.isEmpty() || !isPlaceholder(album);
}

}  // namespace kivo::metadata
