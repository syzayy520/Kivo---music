#pragma once

#include "search_filter_model.h"
#include "search_result_model.h"
#include "../../sources/music/music_file_record.h"

#include <QObject>

class SearchModelBundle final : public QObject {
    Q_OBJECT
    Q_PROPERTY(SearchResultModel* results READ results CONSTANT)
    Q_PROPERTY(SearchFilterModel* filters READ filters CONSTANT)

public:
    explicit SearchModelBundle(
        const QList<MusicFileRecord>& musicRecords,
        QObject* parent = nullptr);

    // Swap in real scan results (UI thread only). No-op when empty.
    void applyMusicRecords(const QList<MusicFileRecord>& musicRecords);

    SearchResultModel* results();
    SearchFilterModel* filters();

private:
    SearchResultModel m_results;
    SearchFilterModel m_filters;
};
