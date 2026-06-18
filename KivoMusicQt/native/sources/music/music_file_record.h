#pragma once

#include <QMetaType>
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

// Carried across the library-scan worker thread boundary via queued signals.
Q_DECLARE_METATYPE(MusicFileRecord)
