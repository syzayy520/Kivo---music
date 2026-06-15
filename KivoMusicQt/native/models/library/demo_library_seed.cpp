#include "demo_library_seed.h"

QList<AlbumListItem> DemoLibrarySeed::albums() {
    return {
        {"Nocturne Signals", "Luna Orbit", "FLAC 96 kHz", 0},
        {"Soft Mornings", "Harbor Lights", "From NAS", 11},
        {"Parallel Lines", "Midtown Echoes", "Local folder", 6},
        {"Red Desert", "Amber Blaze", "Today", 1},
        {"Orbitals", "Silent Harbour", "From NAS", 12},
        {"Cascade", "Frostline", "Today", 7},
        {"Timeless", "Paper Ships", "Local folder", 8},
        {"Bloom", "Alder", "Recent scan", 4},
        {"Lighthouse", "Northline", "Today", 5},
        {"Elevate", "City of Echoes", "Today", 2},
        {"Fragments", "Mosaic Field", "From NAS", 10},
        {"Afterimage", "Arlo Finch", "Local folder", 3},
        {"Blue Room", "Velvet Hours", "Today", 9},
        {"Static Bloom", "Alder", "Recent scan", 13},
        {"Glass Roads", "Northline", "From NAS", 14},
        {"Low Sun", "Frostline", "Today", 15},
    };
}

QList<AlbumListItem> DemoLibrarySeed::pins() {
    return {
        {"Nocturne Signals", "Luna Orbit", "Pinned album", 0},
        {"Work Focus", "Quiet local playlist", "Pinned playlist", 8},
        {"Soft Mornings", "Harbor Lights", "Pinned album", 11},
        {"Night Drive", "Deep station", "Pinned station", 6},
        {"Lighthouse", "Northline", "Pinned album", 5},
        {"Fragments", "Mosaic Field", "Pinned album", 10},
        {"Signal Mix", "Luna Orbit and more", "Pinned mix", 1},
        {"Late Archive", "Recently added from NAS", "Pinned station", 13},
        {"Parallel Lines", "Midtown Echoes", "Pinned album", 6},
        {"Blue Room", "Velvet Hours", "Pinned album", 9},
        {"Glass Roads", "Northline", "Pinned album", 12},
        {"Afterimage", "Arlo Finch", "Pinned album", 3},
        {"Low Sun", "Frostline", "Pinned album", 15},
        {"Static Bloom", "Alder", "Pinned album", 14},
        {"Quiet Rooms", "Harbor Lights", "Pinned station", 4},
        {"Red Desert", "Amber Blaze", "Pinned album", 1},
    };
}

QList<AlbumListItem> DemoLibrarySeed::continueListening() {
    return {
        {"Waves of Yesterday", "Harbor Lights", "Soft Mornings", 11},
        {"Midnight Walks", "Midtown Echoes", "Parallel Lines", 6},
    };
}

QList<AlbumListItem> DemoLibrarySeed::recentAlbums() {
    return {
        {"Red Desert", "Amber Blaze", "Today", 1},
        {"Cascade", "Frostline", "Today", 7},
        {"Lighthouse", "Northline", "Today", 5},
        {"Elevate", "City of Echoes", "Today", 2},
        {"Blue Room", "Velvet Hours", "Today", 9},
        {"Low Sun", "Frostline", "Today", 15},
        {"Bloom", "Alder", "Recent scan", 4},
        {"Static Bloom", "Alder", "Recent scan", 13},
        {"Orbitals", "Silent Harbour", "From NAS", 12},
        {"Glass Roads", "Northline", "From NAS", 14},
        {"Soft Mornings", "Harbor Lights", "From NAS", 11},
        {"Fragments", "Mosaic Field", "From NAS", 10},
    };
}

QList<ArtistListItem> DemoLibrarySeed::artists() {
    return {
        {"Luna Orbit", "12 albums", 0},
        {"Harbor Lights", "8 albums", 11},
        {"Arlo Finch", "6 albums", 4},
        {"Midtown Echoes", "9 albums", 6},
        {"Paper Ships", "5 albums", 8},
        {"Northline", "7 albums", 5},
        {"Frostline", "4 albums", 7},
        {"Alder", "5 albums", 13},
        {"Velvet Hours", "3 albums", 9},
        {"Mosaic Field", "6 albums", 10},
        {"Silent Harbour", "8 albums", 12},
        {"Amber Blaze", "4 albums", 1},
    };
}

QList<TrackListItem> DemoLibrarySeed::songs() {
    return {
        {"Signal in the Dark", "Luna Orbit", "Nocturne Signals", "4:37", 0},
        {"Waves of Yesterday", "Harbor Lights", "Soft Mornings", "4:23", 11},
        {"Midnight Walks", "Midtown Echoes", "Parallel Lines", "6:48", 6},
        {"Falling Through", "Paper Ships", "Timeless", "3:48", 8},
        {"Lighthouse", "Northline", "Lighthouse", "5:12", 5},
        {"Bloom", "Alder", "Bloom", "3:56", 4},
        {"Low Sun", "Frostline", "Cascade", "4:18", 7},
        {"Afterimage", "Arlo Finch", "Afterimage", "3:41", 3},
        {"Glass Roads", "Northline", "Lighthouse", "5:02", 12},
        {"Static Bloom", "Alder", "Static Bloom", "3:27", 13},
        {"Fragments", "Mosaic Field", "Fragments", "4:52", 10},
        {"Blue Room", "Velvet Hours", "Blue Room", "3:33", 9},
    };
}

QList<TrackListItem> DemoLibrarySeed::favorites() {
    return {
        {"Signal in the Dark", "Luna Orbit", "Nocturne Signals", "4:37", 0},
        {"Soft Mornings", "Harbor Lights", "Soft Mornings", "4:23", 11},
        {"Blue Room", "Velvet Hours", "Blue Room", "3:33", 9},
        {"Low Sun", "Frostline", "Cascade", "4:18", 7},
        {"Lighthouse", "Northline", "Lighthouse", "5:12", 5},
        {"Afterimage", "Arlo Finch", "Afterimage", "3:41", 3},
        {"Falling Through", "Paper Ships", "Timeless", "3:48", 8},
        {"Glass Roads", "Northline", "Lighthouse", "5:02", 12},
        {"Fragments", "Mosaic Field", "Fragments", "4:52", 10},
        {"Static Bloom", "Alder", "Static Bloom", "3:27", 13},
        {"Midnight Walks", "Midtown Echoes", "Parallel Lines", "6:48", 6},
        {"Orbitals", "Silent Harbour", "Orbitals", "4:09", 14},
    };
}

QList<TrackListItem> DemoLibrarySeed::focus() {
    return {
        {"Quiet Rooms", "Harbor Lights", "Work Focus", "5:18", 11},
        {"Paper Trails", "Paper Ships", "Work Focus", "4:44", 8},
        {"Long Walks", "Midtown Echoes", "Work Focus", "6:06", 6},
        {"Cascade", "Frostline", "Work Focus", "4:18", 7},
        {"Fragments", "Mosaic Field", "Work Focus", "4:52", 10},
        {"Late Archive", "Alder", "Work Focus", "3:57", 13},
        {"Blue Hour", "Velvet Hours", "Work Focus", "3:33", 9},
        {"Low Sun", "Frostline", "Work Focus", "4:18", 7},
        {"Signal Path", "Luna Orbit", "Work Focus", "4:37", 0},
    };
}

QList<TrackListItem> DemoLibrarySeed::videos() {
    return {
        {"Night Drive Session", "Luna Orbit", "Music Video", "4:37", 0},
        {"Harbor Room Live", "Harbor Lights", "Music Video", "4:23", 11},
        {"Paper Ships Studio", "Paper Ships", "Music Video", "3:48", 8},
        {"Northline Coast", "Northline", "Music Video", "5:12", 5},
        {"Afterimage Cut", "Arlo Finch", "Music Video", "3:41", 3},
        {"Blue Hour Film", "Velvet Hours", "Music Video", "3:33", 9},
        {"Cascade Field", "Frostline", "Music Video", "4:18", 7},
        {"Fragments Live", "Mosaic Field", "Music Video", "4:52", 10},
        {"Static Bloom Film", "Alder", "Music Video", "3:27", 13},
    };
}
