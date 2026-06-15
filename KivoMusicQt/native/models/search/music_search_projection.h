#pragma once

#include "search_result_model.h"
#include "../../sources/music/music_file_record.h"

#include <QList>

class MusicSearchProjection final {
public:
    static QList<SearchResultItem> results(
        const QList<MusicFileRecord>& records,
        int maxItems);
};
