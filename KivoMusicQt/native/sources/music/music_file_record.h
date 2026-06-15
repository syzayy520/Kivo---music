#pragma once

#include <QString>

struct MusicFileRecord {
    QString path;
    QString title;
    QString artist;
    QString album;
    QString sourceLabel;
    QString formatLabel;
    QString coverPath;
    int artVariant = 0;
};
