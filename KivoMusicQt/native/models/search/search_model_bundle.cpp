#include "search_model_bundle.h"

#include "demo_search_seed.h"
#include "music_search_projection.h"

SearchModelBundle::SearchModelBundle(
    const QList<MusicFileRecord>& musicRecords,
    QObject* parent)
    : QObject(parent),
      m_results(),
      m_filters() {
    if (musicRecords.isEmpty()) {
        m_results.setItems(DemoSearchSeed::results());
    } else {
        m_results.setItems(MusicSearchProjection::results(musicRecords, 24));
    }
    m_filters.setItems(DemoSearchSeed::filters());
}

SearchResultModel* SearchModelBundle::results() {
    return &m_results;
}

SearchFilterModel* SearchModelBundle::filters() {
    return &m_filters;
}
