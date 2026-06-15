#pragma once

#include <QFileInfo>
#include <QString>

class MusicCoverFinder final {
public:
    static QString findFor(const QFileInfo& audioFile);
};
