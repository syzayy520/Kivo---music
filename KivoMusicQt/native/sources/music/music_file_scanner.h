#pragma once

#include "music_file_record.h"
#include "music_source_root.h"

#include <QList>

#include <atomic>

class MusicFileScanner final {
public:
    // `cancelled` (optional) lets a worker abort a long/blocking scan promptly
    // (checked between files). Network (UNC) roots are reachability-probed with a
    // short timeout first, so an unreachable NAS is skipped instead of blocking
    // on a multi-second SMB/filesystem timeout.
    static QList<MusicFileRecord> scan(
        const QList<MusicSourceRoot>& roots,
        int maxFiles,
        const std::atomic<bool>* cancelled = nullptr);
};
