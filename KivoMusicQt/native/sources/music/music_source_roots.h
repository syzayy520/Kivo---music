#pragma once

#include "music_source_root.h"

#include <QList>

class MusicSourceRoots final {
public:
    // Built-in roots that always exist regardless of user configuration —
    // currently the OS "Music" library folder. Network roots are NOT hardcoded
    // here; they are user-configured and persisted via
    // kivo::qt::sources::UserSourceRootStore, then merged in by
    // kivo::qt::sources::SourceRootComposer.
    static QList<MusicSourceRoot> builtIn();
};
