#pragma once

#include "music_source_root.h"

#include <QList>

class MusicSourceRoots final {
public:
    static QList<MusicSourceRoot> defaults();
};
