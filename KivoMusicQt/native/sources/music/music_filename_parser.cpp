#include "music_filename_parser.h"

#include <QRegularExpression>

ParsedMusicName MusicFilenameParser::parse(const QString& completeBaseName) {
    auto name = completeBaseName.trimmed();
    name.remove(QRegularExpression(R"(^\d{1,3}[\s._-]+)"));

    const auto split = name.split(" - ", Qt::SkipEmptyParts);
    if (split.size() >= 2) {
        return {split.first().trimmed(), split.last().trimmed()};
    }

    return {name, "Unknown Artist"};
}
