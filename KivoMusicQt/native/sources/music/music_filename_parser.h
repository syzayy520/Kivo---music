#pragma once

#include <QString>

struct ParsedMusicName {
    QString title;
    QString artist;
};

class MusicFilenameParser final {
public:
    static ParsedMusicName parse(const QString& completeBaseName);
};
