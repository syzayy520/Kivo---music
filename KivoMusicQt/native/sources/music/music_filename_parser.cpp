#include "music_filename_parser.h"

#include <QRegularExpression>

// Strip release/format/catalog cruft that pollutes ripped-file names so the UI
// shows a clean title — "15周年紀念精選 (1994) [FLAC] {95488-2}" → "15周年紀念精選 (1994)".
// Square brackets [FLAC]/[24bit]/[Hi-Res] and braces {catalog-no} are almost
// always tags, never part of a song name; round parens (Live)/(Acoustic)/(1994)
// often ARE meaningful, so they are kept.
QString MusicFilenameParser::cleanDisplayField(const QString& raw) {
    static const QRegularExpression brackets(R"(\s*[\[\{][^\]\}]*[\]\}])");
    static const QRegularExpression extraSpace(R"(\s{2,})");
    static const QRegularExpression trailingSep(R"([\s\-_·]+$)");
    QString s = raw;
    s.remove(brackets);
    s.replace(extraSpace, QStringLiteral(" "));
    s.remove(trailingSep);  // drop a separator left dangling after removal
    return s.trimmed();
}

ParsedMusicName MusicFilenameParser::parse(const QString& completeBaseName) {
    auto name = completeBaseName.trimmed();
    name.remove(QRegularExpression(R"(^\d{1,3}[\s._-]+)"));

    // Convention: "<Title> - <Artist>" for downloaded singles (most common in test
    // library). split.first() = Title, split.last() = Artist.
    const auto split = name.split(" - ", Qt::SkipEmptyParts);
    if (split.size() >= 2) {
        return {cleanDisplayField(split.first()), cleanDisplayField(split.last())};
    }

    return {cleanDisplayField(name), "Unknown Artist"};
}
