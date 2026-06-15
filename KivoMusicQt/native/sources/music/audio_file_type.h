#pragma once

#include <QString>

class AudioFileType final {
public:
    static bool isSupported(const QString& suffix);
    static QString labelFor(const QString& suffix);
};
