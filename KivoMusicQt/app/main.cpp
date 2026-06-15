#include <QGuiApplication>
#include <QImage>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlError>
#include <QQuickStyle>
#include <QTimer>

#include <cstdio>

#include "../native/models/library/library_model_bundle.h"
#include "../native/models/search/search_model_bundle.h"
#include "../native/models/transport/transport_state.h"
#include "../native/sources/music/music_file_scanner.h"
#include "../native/sources/music/music_source_roots.h"
#include "../native/audio_core_bridge/audio_playback_controller.hpp"
#include "../native/data/database/database_manager.hpp"

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

} // namespace

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("Kivo Music");
    QGuiApplication::setOrganizationName("Kivo");
    QQuickStyle::setStyle("Basic");

    // ── Initialize SQLite database ──────────────────────────────
    kivo::qt::data::DatabaseManager::instance().initialize();

    // ── Register C++ ImageProvider ──────────────────────────────
    QQmlApplicationEngine engine;
    const auto musicRecords = MusicFileScanner::scan(
        MusicSourceRoots::defaults(),
        768);
    LibraryModelBundle libraryModels(musicRecords);
    SearchModelBundle searchModels(musicRecords);
    TransportState transportState;

    // Create audio playback controller
    auto* audioController = new kivo::qt::audio_bridge::AudioPlaybackController(&app);

    engine.rootContext()->setContextProperty("libraryModels", &libraryModels);
    engine.rootContext()->setContextProperty("searchModels", &searchModels);
    engine.rootContext()->setContextProperty("transportState", &transportState);
    engine.rootContext()->setContextProperty("audioController", audioController);
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

    const auto capturePage = screenshotPage(app.arguments());
    if (!capturePage.isEmpty() && !engine.rootObjects().isEmpty()) {
        engine.rootObjects().constFirst()->setProperty(
            "currentPage",
            capturePage);
    }

    const auto startupOpenPath = openFilePath(app.arguments());
    if (!startupOpenPath.isEmpty()) {
        QTimer::singleShot(200, &app, [audioController, startupOpenPath]() {
            audioController->openFile(startupOpenPath);
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
