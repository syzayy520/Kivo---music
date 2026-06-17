cmake_minimum_required(VERSION 3.24)

project(KivoMusicQt
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

# Set Qt paths explicitly to avoid path encoding issues
# Try KIVO_QT_ROOT env var first, then .qt/ fallback
if(DEFINED ENV{KIVO_QT_ROOT} AND IS_DIRECTORY "$ENV{KIVO_QT_ROOT}")
    set(QT_ROOT "$ENV{KIVO_QT_ROOT}")
    message(STATUS "Using KIVO_QT_ROOT: ${QT_ROOT}")
elseif(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.qt/6.8.3/msvc2022_64")
    set(QT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.qt/6.8.3/msvc2022_64")
else()
    message(FATAL_ERROR "Qt6 not found. Set KIVO_QT_ROOT env var or install Qt to .qt/")
endif()
list(APPEND CMAKE_PREFIX_PATH "${QT_ROOT}")
set(QT_HOST_PATH "${QT_ROOT}")

# QuickEffectsPrivate backs the QtQuick.Effects / MultiEffect usage that the UI
# relies on pervasively (shadows/blur across App, TransportBar, Sidebar, cards…).
# Available on the Qt 6.8 LTS commercial target.
find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Sql Concurrent Network QuickEffectsPrivate LinguistTools)
qt_standard_project_setup(REQUIRES 6.5)
if (COMMAND qt_policy)
    if (${Qt6_VERSION} VERSION_GREATER_EQUAL "6.8")
        qt_policy(SET QTP0004 NEW)
    endif()
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
        # Metadata intelligence (ID3 title/artist field swap detection; report-only, no writes)
        native/metadata_intelligence/validation/metadata_value_validator.hpp
        native/metadata_intelligence/validation/metadata_value_validator.cpp
        native/metadata_intelligence/confidence/swap_detector.hpp
        native/metadata_intelligence/confidence/swap_detector.cpp
        native/metadata_intelligence/repair/swap_corrector.hpp
        native/metadata_intelligence/repair/swap_corrector.cpp
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
        native/sources/music/embedded_cover_extractor.cpp
        native/sources/music/embedded_cover_extractor.h
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
        # Sources (user-configurable source roots: NAS/SMB manual config)
        native/sources/roots/user_source_root.h
        native/sources/roots/user_source_root_store.hpp
        native/sources/roots/user_source_root_store.cpp
        native/sources/roots/source_root_composer.hpp
        native/sources/roots/source_root_composer.cpp
        native/sources/roots/source_manager_controller.hpp
        native/sources/roots/source_manager_controller.cpp
        # Sources (LAN SMB auto-discovery)
        native/sources/discovery/local_subnet.hpp
        native/sources/discovery/local_subnet.cpp
        native/sources/discovery/smb_share_lister.hpp
        native/sources/discovery/smb_share_lister.cpp
        native/sources/discovery/smb_discovery_worker.hpp
        native/sources/discovery/smb_discovery_worker.cpp
        native/sources/discovery/smb_discovery_controller.hpp
        native/sources/discovery/smb_discovery_controller.cpp
        # Services (settings spine: store + catalog registry + facade)
        native/services/settings/settings_scope.hpp
        native/services/settings/settings_store.hpp
        native/services/settings/settings_store.cpp
        native/services/settings/settings_catalog.hpp
        native/services/settings/settings_catalog.cpp
        native/services/settings/settings_controller.hpp
        # Services (lyrics: .lrc parser + sync engine + QML controller)
        native/services/lyrics/lrc_parser.hpp
        native/services/lyrics/lrc_parser.cpp
        native/services/lyrics/lyrics_synchronizer.hpp
        native/services/lyrics/lyrics_synchronizer.cpp
        native/services/lyrics/lyrics_controller.hpp
        native/services/lyrics/lyrics_controller.cpp
        # Services (media keys — E10 WM_APPCOMMAND 系统媒体键拦截)
        native/services/media_keys/media_key_filter.hpp
        native/services/media_keys/media_key_filter.cpp
        # Services (library worker lane)
        native/services/library_index/library_scan_worker.hpp
        native/services/library_index/library_scan_worker.cpp
        native/services/library_index/library_scan_service.hpp
        native/services/library_index/library_scan_service.cpp
        native/services/library_index/library_repository.hpp
        native/services/library_index/library_repository.cpp
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
        # Producer pump (dedicated audio-production thread)
        native/audio_core_bridge/pump/audio_pump_worker.hpp
        native/audio_core_bridge/pump/audio_pump_worker.cpp
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
        native/data/image/cover_color_extractor.hpp
        native/data/image/cover_color_extractor.cpp
)

target_include_directories(KivoMusicQt
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../KivoAudioCoreCpp/include"
)

# Theme is an app-wide design-token SINGLETON (one shared instance → global dark
# mode). Must be marked before qt_add_qml_module registers it in the module.
set_source_files_properties(ui/qml/tokens/Theme.qml
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

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
        ui/qml/windows/mini/MiniHeader.qml
        ui/qml/windows/mini/MiniTrackInfo.qml
        ui/qml/windows/mini/MiniProgressBar.qml
        ui/qml/windows/mini/MiniControls.qml
        ui/qml/windows/NowPlayingView.qml
        ui/qml/pages/home/HomePage.qml
        ui/qml/pages/discovery/DiscoveryPage.qml
        ui/qml/pages/library/LibraryPage.qml
        ui/qml/pages/library/detail/ArtistDetailPage.qml
        ui/qml/pages/library/detail/AlbumDetailPage.qml
        ui/qml/pages/search/SearchPage.qml
        ui/qml/pages/settings/SettingsShell.qml
        ui/qml/pages/settings/shell/SettingsCategoryRail.qml
        ui/qml/pages/settings/shell/SettingsDetailHost.qml
        ui/qml/pages/settings/categories/appearance/AppearancePage.qml
        ui/qml/pages/settings/categories/audio/AudioEnginePage.qml
        ui/qml/pages/settings/categories/sources/SourcesPage.qml
        ui/qml/pages/settings/categories/about/AboutLegalPage.qml
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
        ui/qml/components/library/detail/DetailHero.qml
        ui/qml/components/library/detail/DetailActionBar.qml
        ui/qml/components/library/detail/DetailTrackRow.qml
        ui/qml/components/search/SearchResultRow.qml
        ui/qml/components/navigation/NavigationItem.qml
        ui/qml/components/navigation/NavigationGlyph.qml
        ui/qml/components/navigation/SidebarBrand.qml
        ui/qml/components/navigation/Sidebar.qml
        ui/qml/components/navigation/glyphs/HomeGlyph.qml
        ui/qml/components/navigation/glyphs/SearchGlyph.qml
        ui/qml/components/navigation/glyphs/RecentGlyph.qml
        ui/qml/components/navigation/glyphs/ArtistGlyph.qml
        ui/qml/components/navigation/glyphs/AlbumGlyph.qml
        ui/qml/components/navigation/glyphs/SongGlyph.qml
        ui/qml/components/navigation/glyphs/FavoriteGlyph.qml
        ui/qml/components/navigation/glyphs/SettingsGlyph.qml
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
        ui/qml/components/transport/TransportRightCluster.qml
        ui/qml/components/transport/TransportTrackInfo.qml
        ui/qml/components/transport/VolumeCluster.qml
        ui/qml/components/transport/LyricsDisplay.qml
        ui/qml/components/transport/PlayingPulse.qml
        ui/qml/components/transport/queue/PlayQueueDrawer.qml
        ui/qml/components/transport/queue/PlayQueueHeader.qml
        ui/qml/components/transport/queue/PlayQueueRow.qml
        ui/qml/components/transport/queue/PlayQueueModeBar.qml
        ui/qml/components/transport/glyphs/PlayPauseGlyph.qml
        ui/qml/components/transport/glyphs/PreviousGlyph.qml
        ui/qml/components/transport/glyphs/NextGlyph.qml
        ui/qml/components/transport/glyphs/ShuffleGlyph.qml
        ui/qml/components/transport/glyphs/RepeatGlyph.qml
        ui/qml/components/transport/glyphs/QueueGlyph.qml
        ui/qml/components/transport/glyphs/LyricsGlyph.qml
        ui/qml/components/visualizer/WaveformBar.qml
        ui/qml/components/visualizer/SpectrumView.qml
        ui/qml/components/equalizer/EqSlider.qml
        ui/qml/components/equalizer/EqPanel.qml
        ui/qml/components/truth/SignalPathPanel.qml
        ui/qml/components/truth/SignalPathStage.qml
        ui/qml/components/truth/SignalPathPopover.qml
        ui/qml/components/truth/TruthBadge.qml
        ui/qml/components/common/CustomScrollBar.qml
        ui/qml/components/common/SeekBar.qml
        ui/qml/components/common/LoadingSpinner.qml
        ui/qml/components/common/ErrorDialog.qml
        ui/qml/windows/startup/SplashOverlay.qml
        ui/qml/windows/startup/EulaDialog.qml
        ui/qml/windows/startup/OnboardingDialog.qml
        ui/qml/pages/settings/categories/language/LanguagePage.qml
        ui/qml/components/settings/SettingsSection.qml
        ui/qml/components/settings/KivoSwitch.qml
        ui/qml/components/settings/SettingsToggleRow.qml
        ui/qml/components/settings/SettingsNavRow.qml
        ui/qml/components/settings/SettingsValueRow.qml
        ui/qml/components/settings/SettingsEntityListRow.qml
        ui/qml/components/settings/SettingsSubPage.qml
        ui/qml/components/nowplaying/NowPlayingBackground.qml
        ui/qml/components/nowplaying/NowPlayingArtwork.qml
        ui/qml/components/nowplaying/NowPlayingMeta.qml
        ui/qml/components/nowplaying/NowPlayingScrubber.qml
        ui/qml/components/nowplaying/NowPlayingPlayButton.qml
        ui/qml/components/nowplaying/NowPlayingControls.qml
        ui/qml/components/nowplaying/NowPlayingTruthStrip.qml
        ui/qml/components/nowplaying/NowPlayingCollapseButton.qml
)

# ── i18n: compile .ts → .qm and embed in resources ──────────────────────────
# Source language is zh_CN (the strings in qsTr() are already Chinese).
# kivo_en.ts maps each Chinese source string to English for EN locale users.
qt_add_translations(KivoMusicQt
    IMMEDIATE_CALL
    TS_FILES
        i18n/kivo_zh_CN.ts
        i18n/kivo_en.ts
    RESOURCE_PREFIX "/i18n"
)

target_link_libraries(KivoMusicQt
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Sql
        Qt6::Concurrent
        Qt6::Network
        Qt6::QuickEffectsPrivate
)

if (WIN32)
    # NetShareEnum (LAN SMB share discovery) lives in netapi32.
    target_link_libraries(KivoMusicQt PRIVATE netapi32)
endif()

if (MSVC)
    target_compile_options(KivoMusicQt PRIVATE /W4 /utf-8)
else()
    target_compile_options(KivoMusicQt PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# ── Self-contained deployment ────────────────────────────────────────────────
# Copy the Qt runtime (DLLs, QML, plugins) next to the executable after every
# build so a DIRECT launch (double-click / installed app) finds everything —
# without relying on the Qt bin being on PATH. A stale deployment is exactly how
# a missing Qt6Concurrent.dll slipped through and crashed direct launches, so
# this runs on every build and is required for a store-ready, redistributable app.
if (WIN32)
    # Resolve directly from QT_ROOT (not find_program) so a changed QT_ROOT always
    # re-derives the path instead of sticking to a stale cache entry.
    set(KIVO_WINDEPLOYQT "${QT_ROOT}/bin/windeployqt.exe")
    if (NOT EXISTS "${KIVO_WINDEPLOYQT}")
        # A Windows build that cannot deploy the Qt runtime is not shippable, so
        # fail loudly at configure time rather than silently producing a release
        # artifact that crashes on the target with missing Qt6*.dll.
        message(FATAL_ERROR
            "windeployqt not found at ${KIVO_WINDEPLOYQT}. The build cannot be made "
            "self-contained. Set KIVO_QT_ROOT to a complete Qt 6.8 installation.")
    endif()
    add_custom_command(TARGET KivoMusicQt POST_BUILD
        COMMAND "${KIVO_WINDEPLOYQT}"
                --release
                --no-translations
                --no-compiler-runtime
                --qmldir "${CMAKE_CURRENT_SOURCE_DIR}/ui/qml"
                "$<TARGET_FILE:KivoMusicQt>"
        COMMENT "windeployqt: deploying Qt runtime for a self-contained build"
        VERBATIM)

    # The audio engine (kivo_audio_core.dll) and its FFmpeg runtime are produced by
    # the SEPARATE KivoAudioCoreCpp project and loaded at runtime via LoadLibrary.
    # windeployqt only knows about Qt, so without this copy a redistributed build
    # has no engine and ALL playback fails (AudioCoreLibraryGuard returns DllLoad).
    # This is the DLL that makes Kivo a music player — locate it or fail the build.
    set(KIVO_AUDIO_CORE_HINTS
        "${CMAKE_CURRENT_SOURCE_DIR}/../KivoAudioCoreCpp/out/local-release/Release"
        "${CMAKE_CURRENT_SOURCE_DIR}/../KivoAudioCoreCpp/out/local-release"
        "${CMAKE_CURRENT_SOURCE_DIR}/../KivoAudioCoreCpp/build/Release")
    find_file(KIVO_AUDIO_CORE_DLL
        NAMES kivo_audio_core.dll
        HINTS ${KIVO_AUDIO_CORE_HINTS}
        NO_DEFAULT_PATH)
    if (NOT KIVO_AUDIO_CORE_DLL)
        message(FATAL_ERROR
            "kivo_audio_core.dll not found in any of: ${KIVO_AUDIO_CORE_HINTS}. "
            "Build the KivoAudioCoreCpp project (Release) first — the app cannot "
            "play audio without it.")
    endif()
    get_filename_component(KIVO_AUDIO_CORE_DIR "${KIVO_AUDIO_CORE_DLL}" DIRECTORY)
    # Copy the engine plus every FFmpeg DLL that sits beside it (glob is robust to
    # FFmpeg version bumps, e.g. avcodec-61 -> avcodec-62).
    file(GLOB KIVO_ENGINE_RUNTIME
        "${KIVO_AUDIO_CORE_DIR}/av*.dll"
        "${KIVO_AUDIO_CORE_DIR}/sw*.dll")
    add_custom_command(TARGET KivoMusicQt POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${KIVO_AUDIO_CORE_DLL}" ${KIVO_ENGINE_RUNTIME}
                "$<TARGET_FILE_DIR:KivoMusicQt>"
        COMMENT "Deploying audio engine + FFmpeg runtime next to the executable"
        VERBATIM)
endif()
