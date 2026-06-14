#pragma once

#include "music_file_record.h"
#include "music_source_root.h"

#include <QList>

class MusicFileScanner final {
public:
    static QList<MusicFileRecord> scan(
        const QList<MusicSourceRoot>& roots,
        int maxFiles);
};
