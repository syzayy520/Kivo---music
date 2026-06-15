cmake_minimum_required(VERSION 3.24)

project(KivoMusicQt
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set Qt paths explicitly to avoid path encoding issues
set(QT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.qt/6.8.3/msvc2022_64")
list(APPEND CMAKE_PREFIX_PATH "${QT_ROOT}")
set(QT_HOST_PATH "${QT_ROOT}")

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Sql QuickEffectsPrivate)
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
        native/models/library/home/home_curation_projection.h
        native/models/library/home/home_curation_helpers.h
        native/models/library/home/home_curation_helpers.cpp
        native/models/library/home/home_curation_top_picks.cpp
        native/models/library/home/home_curation_recently_added.cpp
        native/models/library/home/home_curation_rediscover.cpp
        native/models/library/home/home_curation_after_hours.cpp
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
        # AudioCoreBridge
        native/audio_core_bridge/error/bridge_error.hpp
        native/audio_core_bridge/loader/audio_core_library_loader.hpp
        native/audio_core_bridge/loader/audio_core_library_loader.cpp
        native/audio_core_bridge/loader/audio_core_function_resolver.hpp
        native/audio_core_bridge/loader/audio_core_function_resolver.cpp
        native/audio_core_bridge/loader/audio_core_library_guard.hpp
        native/audio_core_bridge/loader/audio_core_library_guard.cpp
        native/audio_core_bridge/handle/engine_handle.hpp
        native/audio_core_bridge/handle/engine_handle.cpp
        native/audio_core_bridge/source/file_audio_source.hpp
        native/audio_core_bridge/source/file_audio_source.cpp
        native/audio_core_bridge/state/bridge_session_clock.hpp
        native/audio_core_bridge/state/bridge_session_clock.cpp
        native/audio_core_bridge/state/default_open_configuration.hpp
        native/audio_core_bridge/state/default_open_configuration.cpp
        native/audio_core_bridge/snapshot/playback_snapshot.hpp
        native/audio_core_bridge/snapshot/playback_snapshot.cpp
        native/audio_core_bridge/snapshot/format_snapshot.hpp
        native/audio_core_bridge/snapshot/format_snapshot.cpp
        native/audio_core_bridge/snapshot/truth_snapshot.hpp
        native/audio_core_bridge/snapshot/truth_snapshot.cpp
        native/audio_core_bridge/command/playback_command.hpp
        native/audio_core_bridge/command/playback_command.cpp
        native/audio_core_bridge/command/seek_command.hpp
        native/audio_core_bridge/command/seek_command.cpp
        native/audio_core_bridge/command/volume_command.hpp
        native/audio_core_bridge/command/volume_command.cpp
        native/audio_core_bridge/audio_playback_controller.hpp
        native/audio_core_bridge/audio_playback_controller.cpp
        native/audio_core_bridge/controller/audio_playback_controller_impl.hpp
        native/audio_core_bridge/controller/audio_playback_controller_impl_lifecycle.cpp
        native/audio_core_bridge/controller/audio_playback_controller_impl_open.cpp
        native/audio_core_bridge/controller/audio_playback_controller_impl_queue.cpp
        native/audio_core_bridge/controller/audio_playback_controller_impl_commands.cpp
        native/audio_core_bridge/controller/audio_playback_controller_impl_snapshot.cpp
        native/audio_core_bridge/controller/audio_playback_controller_impl_properties.cpp
        # Playback Queue and Mode
        native/audio_core_bridge/queue/play_queue.hpp
        native/audio_core_bridge/queue/play_queue.cpp
        native/audio_core_bridge/queue/playback_mode.hpp
        native/audio_core_bridge/queue/playback_mode.cpp
        # Lyrics
        native/audio_core_bridge/lyrics/lyrics_parser.hpp
        native/audio_core_bridge/lyrics/lyrics_parser.cpp
        native/audio_core_bridge/lyrics/lyrics_fetcher.hpp
        native/audio_core_bridge/lyrics/lyrics_fetcher.cpp
        # EQ
        native/audio_core_bridge/eq/eq_band.hpp
        native/audio_core_bridge/eq/eq_preset.hpp
        native/audio_core_bridge/eq/eq_preset.cpp
        # Database
        native/data/database/database_manager.hpp
        native/data/database/database_manager.cpp
        native/data/database/dao/playlist_dao.hpp
        native/data/database/dao/playlist_dao.cpp
        # Image Provider
        native/data/image/cover_image_provider.hpp
        native/data/image/cover_image_provider.cpp
)

target_include_directories(KivoMusicQt
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../KivoAudioCoreCpp/include"
)

qt_add_qml_module(KivoMusicQt
    URI KivoMusic
    VERSION 1.0
    NO_IMPORT_SCAN
    QML_FILES
        ui/qml/App.qml
        ui/qml/tokens/Theme.qml
        ui/qml/animations/PageTransition.qml
        ui/qml/animations/EntranceAnimation.qml
        ui/qml/windows/HomeWindow.qml
        ui/qml/windows/MiniPlayer.qml
        ui/qml/pages/home/HomePage.qml
        ui/qml/pages/discovery/DiscoveryPage.qml
        ui/qml/pages/library/LibraryPage.qml
        ui/qml/pages/search/SearchPage.qml
        ui/qml/pages/settings/SettingsPage.qml
        ui/qml/pages/settings/PrivacyPolicy.qml
        ui/qml/pages/settings/EulaPage.qml
        ui/qml/pages/settings/AboutPage.qml
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
        ui/qml/components/shell/OpenFileButton.qml
        ui/qml/components/shell/WindowDragRegion.qml
        ui/qml/components/shell/WindowControlButton.qml
        ui/qml/components/shell/ThemeToggleButton.qml
        ui/qml/components/shell/KeyboardShortcuts.qml
        ui/qml/components/shell/FileDropArea.qml
        ui/qml/components/transport/AudioStatusText.qml
        ui/qml/components/transport/PlaybackControlCluster.qml
        ui/qml/components/transport/TransportBar.qml
        ui/qml/components/transport/TransportButton.qml
        ui/qml/components/transport/TransportTrackInfo.qml
        ui/qml/components/transport/VolumeCluster.qml
        ui/qml/components/transport/LyricsDisplay.qml
        ui/qml/components/transport/PlayQueuePanel.qml
        ui/qml/components/visualizer/WaveformBar.qml
        ui/qml/components/equalizer/EqSlider.qml
        ui/qml/components/equalizer/EqPanel.qml
        ui/qml/components/truth/SignalPathPanel.qml
        ui/qml/components/truth/TruthBadge.qml
        ui/qml/components/common/CustomScrollBar.qml
)

target_link_libraries(KivoMusicQt
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Sql
        Qt6::QuickEffectsPrivate
)

if (MSVC)
    target_compile_options(KivoMusicQt PRIVATE /W4 /utf-8)
else()
    target_compile_options(KivoMusicQt PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
