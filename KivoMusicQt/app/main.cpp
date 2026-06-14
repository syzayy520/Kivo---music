#include <QGuiApplication>
#include <QImage>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QTimer>

#include "../native/models/library/library_model_bundle.h"
#include "../native/models/search/search_model_bundle.h"
#include "../native/models/transport/transport_state.h"
#include "../native/sources/music/music_file_scanner.h"
#include "../native/sources/music/music_source_roots.h"

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

} // namespace

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName("Kivo Music");
    QGuiApplication::setOrganizationName("Kivo");
    QQuickStyle::setStyle("Basic");

    QQmlApplicationEngine engine;
    const auto musicRecords = MusicFileScanner::scan(
        MusicSourceRoots::defaults(),
        768);
    LibraryModelBundle libraryModels(musicRecords);
    SearchModelBundle searchModels(musicRecords);
    TransportState transportState;
    engine.rootContext()->setContextProperty("libraryModels", &libraryModels);
    engine.rootContext()->setContextProperty("searchModels", &searchModels);
    engine.rootContext()->setContextProperty("transportState", &transportState);
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

    const auto capturePath = screenshotPath(app.arguments());
    if (!capturePath.isEmpty()) {
        QTimer::singleShot(1200, &app, [&engine, capturePath]() {
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
