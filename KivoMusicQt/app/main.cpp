#include <QGuiApplication>
#include <QImage>
#include <QLocale>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlError>
#include <QQuickStyle>
#include <QSettings>
#include <QTimer>
#include <QTranslator>

#include <cstdio>

#include "../native/models/library/library_model_bundle.h"
#include "../native/models/search/search_model_bundle.h"
#include "../native/models/transport/transport_state.h"
#include "../native/sources/roots/source_manager_controller.hpp"
#include "../native/sources/discovery/smb_discovery_controller.hpp"
#include "../native/services/settings/settings_catalog.hpp"
#include "../native/services/settings/settings_controller.hpp"
#include "../native/services/lyrics/lyrics_controller.hpp"
#include "../native/services/library_index/library_scan_service.hpp"
#include "../native/services/library_index/library_repository.hpp"
#include "../native/audio_core_bridge/audio_playback_controller.hpp"
#include "../native/data/database/database_manager.hpp"
#include "../native/services/media_keys/media_key_filter.hpp"
#include "../native/data/image/cover_image_provider.hpp"

namespace {

QString screenshotPath(const QStringList& arguments) {
    const auto index = arguments.indexOf("--screenshot");
    if (index < 0 || index + 1 >= arguments.size()) {
        return {};
    }
    return arguments.at(index + 1);
}

QString screenshotPage(const QStringList& arguments) {
    const auto index = arguments.indexOf("--screenshot-page");
    if (index < 0 || index + 1 >= arguments.size()) {
        return {};
    }
    return arguments.at(index + 1);
}

QString openFilePath(const QStringList& arguments) {
    const auto index = arguments.indexOf("--open-file");
    if (index < 0 || index + 1 >= arguments.size()) {
        return {};
    }
    return arguments.at(index + 1);
}

int screenshotDelayMs(const QStringList& arguments) {
    const auto index = arguments.indexOf("--screenshot-delay-ms");
    if (index < 0 || index + 1 >= arguments.size()) {
        return 1200;
    }

    bool ok = false;
    const int delay = arguments.at(index + 1).toInt(&ok);
    return ok && delay > 0 ? delay : 1200;
}

// Resolve the UI locale. Priority: --lang CLI override (testing / power users) >
// the user's persisted choice in Settings > the OS locale. English is the SOURCE
// language, so an unmatched locale falls back to readable English, never garbage.
QLocale resolveLocale(const QStringList& arguments) {
    const auto index = arguments.indexOf("--lang");
    if (index >= 0 && index + 1 < arguments.size()) {
        return QLocale(arguments.at(index + 1));
    }
    const QString saved = QSettings().value(QStringLiteral("language")).toString();
    if (!saved.isEmpty()) {
        return QLocale(saved);
    }
    return QLocale::system();
}

} // namespace

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("Kivo Music");
    QGuiApplication::setOrganizationName("Kivo");
    QQuickStyle::setStyle("Basic");

    // E7 i18n: load locale-matched translation (EN users get kivo_en.qm;
    // zh_CN users get kivo_zh_CN.qm which is an identity map).
    QTranslator translator;
    if (translator.load(resolveLocale(app.arguments()), "kivo", "_", ":/i18n")) {
        app.installTranslator(&translator);
    }

    qRegisterMetaType<MusicFileRecord>();
    qRegisterMetaType<QList<MusicFileRecord>>();

    // ── SQLite: initialize + migrate, then seed models from the persisted
    //    library so a cold launch shows the REAL library instantly. The async
    //    rescan below refreshes it; an empty DB falls back to demo shelves. ──
    auto& db = kivo::qt::data::DatabaseManager::instance();
    db.initialize();
    db.migrate(kivo::qt::data::DatabaseManager::kLatestSchemaVersion);
    const QList<MusicFileRecord> seedRecords =
        kivo::qt::services::LibraryRepository(QStringLiteral("kivo_main"))
            .allTracks();

    QQmlApplicationEngine engine;

    // E6: register artwork image provider ("image://artwork/<local-file-path>").
    // Qt takes ownership of the pointer; we keep a non-owning alias for preload calls.
    auto* artworkProvider = new kivo::qt::data::CoverImageProvider();
    engine.addImageProvider(QStringLiteral("artwork"), artworkProvider);
    kivo::qt::data::setGlobalArtworkProvider(artworkProvider);

    LibraryModelBundle libraryModels(seedRecords);
    SearchModelBundle searchModels(seedRecords);
    TransportState transportState;

    // Create audio playback controller
    auto* audioController = new kivo::qt::audio_bridge::AudioPlaybackController(&app);

    // E10 系统媒体键: WM_APPCOMMAND 转发到 audioController slots。
    // 拦截 Play/Pause / Next / Previous / Stop 键, 不阻断其他应用的媒体键处理。
    auto* mediaKeys = new kivo::qt::services::MediaKeyFilter(&app);
    app.installNativeEventFilter(mediaKeys);
    QObject::connect(mediaKeys, &kivo::qt::services::MediaKeyFilter::playPauseRequested,
        audioController, &kivo::qt::audio_bridge::AudioPlaybackController::togglePlayPause);
    QObject::connect(mediaKeys, &kivo::qt::services::MediaKeyFilter::nextTrackRequested,
        audioController, &kivo::qt::audio_bridge::AudioPlaybackController::playNext);
    QObject::connect(mediaKeys, &kivo::qt::services::MediaKeyFilter::previousTrackRequested,
        audioController, &kivo::qt::audio_bridge::AudioPlaybackController::playPrevious);
    QObject::connect(mediaKeys, &kivo::qt::services::MediaKeyFilter::stopRequested,
        audioController, &kivo::qt::audio_bridge::AudioPlaybackController::stop);

    // Music sources: built-in Local Music + user-configured network folders
    // (persisted via QSettings). Replaces the old hardcoded NAS path.
    auto* sourceManager =
        new kivo::qt::sources::SourceManagerController(&app);
    // LAN SMB auto-discovery — promotes discovered shares into sourceManager.
    auto* discoveryController =
        new kivo::qt::sources::SmbDiscoveryController(sourceManager, &app);
    // Settings spine: category registry (drives the shell rail/search) + a
    // generic key/value facade for simple page-level preferences.
    auto* settingsCatalog = new kivo::qt::settings::SettingsCatalog(&app);
    auto* settingsController = new kivo::qt::settings::SettingsController(&app);

    // Lyrics: .lrc parser + sync engine
    auto* lyricsController = new kivo::qt::lyrics::LyricsController(&app);

    engine.rootContext()->setContextProperty("libraryModels", &libraryModels);
    engine.rootContext()->setContextProperty("searchModels", &searchModels);
    engine.rootContext()->setContextProperty("transportState", &transportState);
    engine.rootContext()->setContextProperty("audioController", audioController);
    engine.rootContext()->setContextProperty("sourceManager", sourceManager);
    engine.rootContext()->setContextProperty("discovery", discoveryController);
    engine.rootContext()->setContextProperty("settingsCatalog", settingsCatalog);
    engine.rootContext()->setContextProperty("settingsController", settingsController);
    engine.rootContext()->setContextProperty("lyricsController", lyricsController);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::warnings,
        &app,
        [](const QList<QQmlError>& warnings) {
            for (const QQmlError& warning : warnings) {
                std::fprintf(
                    stderr,
                    "QML warning: %s:%d:%d: %s\n",
                    warning.url().toString().toUtf8().constData(),
                    warning.line(),
                    warning.column(),
                    warning.description().toUtf8().constData());
            }
            std::fflush(stderr);
        });
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("KivoMusic", "App");

    // ── Library scan off the UI thread ──────────────────────────
    // Window is already painting (demo shelves); real results swap in when ready.
    kivo::qt::services::LibraryScanService libraryScanService;
    QObject::connect(
        &libraryScanService,
        &kivo::qt::services::LibraryScanService::resultsReady,
        &app,
        [&libraryModels, &searchModels](const QList<MusicFileRecord>& records) {
            libraryModels.applyMusicRecords(records);
            searchModels.applyMusicRecords(records);
        });
    const QString dbPath = db.databasePath();
    libraryScanService.start(sourceManager->effectiveRoots(), 768, dbPath);
    // Re-scan whenever the user edits their music sources (add/remove/toggle).
    QObject::connect(
        sourceManager,
        &kivo::qt::sources::SourceManagerController::sourcesChanged,
        &app,
        [&libraryScanService, sourceManager, dbPath]() {
            libraryScanService.start(
                sourceManager->effectiveRoots(), 768, dbPath);
        });

    const auto capturePage = screenshotPage(app.arguments());
    if (!capturePage.isEmpty() && !engine.rootObjects().isEmpty()) {
        // Defer past the splash (~1200ms) AND the page entrance animation:
        // switchPage() ignores navigation while the initial fade-in is still
        // running (opacity < 1), so an earlier set would be silently dropped.
        QObject* rootObject = engine.rootObjects().constFirst();
        QTimer::singleShot(1800, rootObject, [rootObject, capturePage]() {
            rootObject->setProperty("currentPage", capturePage);
        });
    }

    const auto startupOpenPath = openFilePath(app.arguments());
    if (!startupOpenPath.isEmpty()) {
        QTimer::singleShot(200, &app, [audioController, startupOpenPath]() {
            audioController->openFile(startupOpenPath);
        });
    }

    // Launch straight into the immersive Now-Playing view (test hook + a genuine
    // "resume where I was" entry point). Deferred past the splash + entrance.
    if (app.arguments().contains("--open-now-playing")
        && !engine.rootObjects().isEmpty()) {
        QObject* root = engine.rootObjects().constFirst();
        QTimer::singleShot(1700, root, [root]() {
            if (auto* np = root->findChild<QObject*>("nowPlayingView")) {
                np->setProperty("open", true);
            }
        });
    }

    // Reveal the signal-path chain (screenshot/verification hook). Context-aware:
    // if the immersive Now-Playing view is open, expand the chain THERE; otherwise
    // pop the transport popover.
    if (app.arguments().contains("--open-signal-path")
        && !engine.rootObjects().isEmpty()) {
        QObject* spRoot = engine.rootObjects().constFirst();
        QTimer::singleShot(2400, spRoot, [spRoot]() {
            auto* np = spRoot->findChild<QObject*>("nowPlayingView");
            if (np && np->property("open").toBool()) {
                np->setProperty("truthExpanded", true);
            } else if (auto* pop = spRoot->findChild<QObject*>("signalPathPopover")) {
                QMetaObject::invokeMethod(pop, "open");
            }
        });
    }

    // Type a search query (screenshot hook; pair with --screenshot-page search).
    {
        const int si = app.arguments().indexOf("--search");
        if (si >= 0 && si + 1 < app.arguments().size()
            && !engine.rootObjects().isEmpty()) {
            const QString queryText = app.arguments().at(si + 1);
            QObject* sRoot = engine.rootObjects().constFirst();
            QTimer::singleShot(6500, sRoot, [sRoot, queryText]() {
                if (auto* f = sRoot->findChild<QObject*>("searchField")) {
                    f->setProperty("text", queryText);
                }
            });
        }
    }

    // Drill into an artist detail view (screenshot hook; pair with
    // --screenshot-page artists). Deferred past the page switch.
    {
        const int di = app.arguments().indexOf("--detail-artist");
        if (di >= 0 && di + 1 < app.arguments().size()
            && !engine.rootObjects().isEmpty()) {
            const QString artist = app.arguments().at(di + 1);
            QObject* daRoot = engine.rootObjects().constFirst();
            // Late enough that the async library scan has populated the records.
            QTimer::singleShot(6500, daRoot, [daRoot, artist]() {
                if (auto* lp = daRoot->findChild<QObject*>("libraryPage")) {
                    lp->setProperty("detailArtist", artist);
                }
            });
        }
    }

    // Play a whole artist (verification hook for playTracks: replace-queue + play).
    {
        const int pi = app.arguments().indexOf("--play-artist");
        if (pi >= 0 && pi + 1 < app.arguments().size()) {
            const QString name = app.arguments().at(pi + 1);
            QTimer::singleShot(6500, &app, [&libraryModels, audioController, name]() {
                const QVariantList rows = libraryModels.tracksForArtist(name);
                QStringList paths;
                for (const QVariant& v : rows) {
                    paths << v.toMap().value(QStringLiteral("filePath")).toString();
                }
                if (!paths.isEmpty()) audioController->playTracks(paths, 0);
            });
        }
    }

    // Open the play-queue drawer (screenshot hook).
    if (app.arguments().contains("--open-queue") && !engine.rootObjects().isEmpty()) {
        QObject* qRoot = engine.rootObjects().constFirst();
        QTimer::singleShot(7400, qRoot, [qRoot]() {
            if (auto* d = qRoot->findChild<QObject*>("playQueueDrawer")) {
                d->setProperty("open", true);
            }
        });
    }

    const auto capturePath = screenshotPath(app.arguments());
    if (!capturePath.isEmpty()) {
        QTimer::singleShot(
            screenshotDelayMs(app.arguments()),
            &app,
            [&engine, capturePath]() {
            if (engine.rootObjects().isEmpty()) {
                QCoreApplication::exit(2);
                return;
            }

            auto* window = qobject_cast<QQuickWindow*>(
                engine.rootObjects().constFirst());
            if (!window) {
                QCoreApplication::exit(3);
                return;
            }

            const QImage image = window->grabWindow();
            if (image.isNull() || !image.save(capturePath)) {
                QCoreApplication::exit(4);
                return;
            }
            QCoreApplication::exit(0);
        });
    }

    return app.exec();
}
