// =============================================================================
// Kivo Music Qt - lyrics_controller.cpp
// =============================================================================

#include "lyrics_controller.hpp"
#include "lrc_parser.hpp"

#include <QFile>
#include <QTextStream>

namespace kivo::qt::lyrics {

LyricsController::LyricsController(QObject* parent)
    : QObject(parent) {
}

void LyricsController::loadLrc(const QString& lrcContent) {
    const auto lines = LrcParser::parse(lrcContent);
    synchronizer_ = std::make_unique<LyricsSynchronizer>(lines);

    currentLine_.clear();
    nextLine_.clear();
    lastCurrentIndex_ = -1;

    emit hasLyricsChanged();
    emit currentLineChanged();
    emit nextLineChanged();
}

void LyricsController::loadLrcFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        clear();
        return;
    }
    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    loadLrc(in.readAll());
}

void LyricsController::clear() {
    synchronizer_.reset();
    currentLine_.clear();
    nextLine_.clear();
    lastCurrentIndex_ = -1;

    emit hasLyricsChanged();
    emit currentLineChanged();
    emit nextLineChanged();
}

void LyricsController::updatePosition(qint64 positionMs) {
    if (!synchronizer_ || synchronizer_->isEmpty()) {
        return;
    }
    syncLines(positionMs);
}

void LyricsController::syncLines(qint64 positionMs) {
    const auto result = synchronizer_->syncAt(positionMs);

    // 只在行切换时更新，避免每次 tick 都触发信号
    if (result.currentIndex != lastCurrentIndex_) {
        lastCurrentIndex_ = result.currentIndex;

        currentLine_ = (result.currentIndex >= 0)
            ? synchronizer_->lines().at(result.currentIndex).text
            : QString();

        nextLine_ = (result.nextIndex >= 0)
            ? synchronizer_->lines().at(result.nextIndex).text
            : QString();

        emit currentLineChanged();
        emit nextLineChanged();
    }
}

} // namespace kivo::qt::lyrics
