#include "demo_search_seed.h"

QList<SearchResultItem> DemoSearchSeed::results() {
    return {
        {"Signal in the Dark", "Luna Orbit - Nocturne Signals", "Song", 0},
        {"Nocturne Signals", "Luna Orbit", "Album", 0},
        {"Midnight Walks", "Midtown Echoes - Parallel Lines", "Song", 6},
        {"Harbor Lights", "8 albums in library", "Artist", 11},
        {"Work Focus", "Quiet local playlist", "Playlist", 8},
        {"Fragments", "Mosaic Field - Local folder", "Album", 10},
        {"Northline Coast", "Northline", "Music Video", 5},
    };
}

QList<SearchFilterItem> DemoSearchSeed::filters() {
    return {
        {"Songs"},
        {"Albums"},
        {"Artists"},
        {"Playlists"},
        {"Music Videos"},
        {"NAS Sources"},
    };
}
