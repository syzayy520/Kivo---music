cmake_minimum_required(VERSION 3.24)

project(KivoMusicQt
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)
qt_standard_project_setup(REQUIRES 6.5)
if (COMMAND qt_policy)
    qt_policy(SET QTP0004 NEW)
endif()

qt_add_executable(KivoMusicQt
    app/main.cpp
)

target_sources(KivoMusicQt
    PRIVATE
        native/models/common/local_file_url.cpp
        native/models/common/local_file_url.h
        native/models/library/album_list_model.cpp
        native/models/library/album_list_model.h
        native/models/library/artist_list_model.cpp
        native/models/library/artist_list_model.h
        native/models/library/demo_library_seed.cpp
        native/models/library/demo_library_seed.h
        native/models/library/home/home_curation_projection.cpp
        native/models/library/home/home_curation_projection.h
        native/models/library/library_model_bundle.cpp
        native/models/library/library_model_bundle.h
        native/models/library/music_library_projection.cpp
        native/models/library/music_library_projection.h
        native/models/library/track_list_model.cpp
        native/models/library/track_list_model.h
        native/models/search/demo_search_seed.cpp
        native/models/search/demo_search_seed.h
        native/models/search/search_filter_model.cpp
        native/models/search/search_filter_model.h
        native/models/search/search_model_bundle.cpp
        native/models/search/search_model_bundle.h
        native/models/search/music_search_projection.cpp
        native/models/search/music_search_projection.h
        native/models/search/search_result_model.cpp
        native/models/search/search_result_model.h
        native/models/transport/transport_state.cpp
        native/models/transport/transport_state.h
        native/sources/music/audio_file_type.cpp
        native/sources/music/audio_file_type.h
        native/sources/music/music_cover_finder.cpp
        native/sources/music/music_cover_finder.h
        native/sources/music/music_file_record.h
        native/sources/music/music_file_scanner.cpp
        native/sources/music/music_file_scanner.h
        native/sources/music/music_filename_parser.cpp
        native/sources/music/music_filename_parser.h
        native/sources/music/music_source_root.h
        native/sources/music/music_source_roots.cpp
        native/sources/music/music_source_roots.h
)

qt_add_qml_module(KivoMusicQt
    URI KivoMusic
    VERSION 1.0
    QML_FILES
        ui/qml/App.qml
        ui/qml/tokens/Theme.qml
        ui/qml/windows/HomeWindow.qml
        ui/qml/pages/home/HomePage.qml
        ui/qml/pages/discovery/DiscoveryPage.qml
        ui/qml/pages/library/LibraryPage.qml
        ui/qml/pages/search/SearchPage.qml
        ui/qml/components/artwork/AlbumArtwork.qml
        ui/qml/components/artwork/GeneratedAlbumArtwork.qml
        ui/qml/components/content/AlbumTile.qml
        ui/qml/components/content/AlbumShelfSection.qml
        ui/qml/components/content/ContinueListeningCard.qml
        ui/qml/components/content/ContinueListeningSection.qml
        ui/qml/components/content/HomeRecommendationSection.qml
        ui/qml/components/content/LibraryAddedSection.qml
        ui/qml/components/content/RecentListenStrip.qml
        ui/qml/components/content/RecentListeningSection.qml
        ui/qml/components/content/ShelfEdgeFade.qml
        ui/qml/components/content/TopPickCard.qml
        ui/qml/components/discovery/DiscoveryCard.qml
        ui/qml/components/library/ArtistTile.qml
        ui/qml/components/library/TrackRow.qml
        ui/qml/components/search/SearchResultRow.qml
        ui/qml/components/navigation/NavigationItem.qml
        ui/qml/components/navigation/NavigationGlyph.qml
        ui/qml/components/navigation/SidebarBrand.qml
        ui/qml/components/navigation/Sidebar.qml
        ui/qml/components/shell/TopBar.qml
        ui/qml/components/shell/WindowDragRegion.qml
        ui/qml/components/shell/WindowControlButton.qml
        ui/qml/components/transport/AudioStatusText.qml
        ui/qml/components/transport/PlaybackControlCluster.qml
        ui/qml/components/transport/TransportBar.qml
        ui/qml/components/transport/TransportButton.qml
        ui/qml/components/transport/TransportTrackInfo.qml
        ui/qml/components/transport/VolumeCluster.qml
        ui/qml/components/truth/SignalPathPanel.qml
        ui/qml/components/truth/TruthBadge.qml
)

target_link_libraries(KivoMusicQt
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
)

if (MSVC)
    target_compile_options(KivoMusicQt PRIVATE /W4 /WX /utf-8)
else()
    target_compile_options(KivoMusicQt PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
