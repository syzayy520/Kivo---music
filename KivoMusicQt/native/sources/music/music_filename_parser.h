#pragma once

#include <QString>

struct ParsedMusicName {
    QString title;
    QString artist;
};

class MusicFilenameParser final {
public:
    static ParsedMusicName parse(const QString& completeBaseName);

    // Strips release/format/catalog cruft ([FLAC], [24bit], {catalog-no}, …) from
    // a display string while keeping meaningful round-paren qualifiers like
    // (Live)/(Acoustic)/(1994). Reused by the scanner to clean album/folder names.
    static QString cleanDisplayField(const QString& raw);
};
