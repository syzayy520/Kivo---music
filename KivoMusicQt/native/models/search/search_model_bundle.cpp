#include "search_model_bundle.h"

#include "demo_search_seed.h"
#include "music_search_projection.h"

SearchModelBundle::SearchModelBundle(
    const QList<MusicFileRecord>& musicRecords,
    QObject* parent)
    : QObject(parent),
      m_results(),
      m_filters() {
    // Seed demo so search has content immediately; real results override below
    // and later via applyMusicRecords().
    m_results.setItems(DemoSearchSeed::results());
    m_filters.setItems(DemoSearchSeed::filters());
    applyMusicRecords(musicRecords);
}

void SearchModelBundle::applyMusicRecords(
    const QList<MusicFileRecord>& musicRecords) {
    if (musicRecords.isEmpty()) {
        return;
    }
    m_results.setItems(MusicSearchProjection::results(musicRecords, 24));
}

SearchResultModel* SearchModelBundle::results() {
    return &m_results;
}

SearchFilterModel* SearchModelBundle::filters() {
    return &m_filters;
}
