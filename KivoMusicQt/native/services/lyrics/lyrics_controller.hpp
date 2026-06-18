// =============================================================================
// Kivo Music Qt - lyrics_controller.hpp
// 职责：QML桥接层 — 监听播放位置变化，更新 currentLine/nextLine Q_PROPERTY。
// 单一职责：只接线 audioController → synchronizer → QML，不含解析/同步算法。
// =============================================================================

#pragma once

#include "lyrics_synchronizer.hpp"

#include <QObject>
#include <QTimer>
#include <memory>

namespace kivo::qt::lyrics {

class LyricsController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString currentLine READ currentLine NOTIFY currentLineChanged)
    Q_PROPERTY(QString nextLine READ nextLine NOTIFY nextLineChanged)
    Q_PROPERTY(bool hasLyrics READ hasLyrics NOTIFY hasLyricsChanged)

public:
    explicit LyricsController(QObject* parent = nullptr);

    QString currentLine() const { return currentLine_; }
    QString nextLine() const { return nextLine_; }
    bool hasLyrics() const { return synchronizer_ && !synchronizer_->isEmpty(); }

    // 加载 .lrc 文件内容（调用 LrcParser → LyricsSynchronizer）
    Q_INVOKABLE void loadLrc(const QString& lrcContent);

    // 直接读取 .lrc 文件路径（C++ QFile 读取，绕过 QML XHR 限制）
    Q_INVOKABLE void loadLrcFile(const QString& filePath);

    // 清空歌词
    Q_INVOKABLE void clear();

    // 更新同步位置（由外部调用，通常是 audioController.elapsedMs 变化时）
    Q_INVOKABLE void updatePosition(qint64 positionMs);

signals:
    void currentLineChanged();
    void nextLineChanged();
    void hasLyricsChanged();

private:
    void syncLines(qint64 positionMs);

    std::unique_ptr<LyricsSynchronizer> synchronizer_;
    QString currentLine_;
    QString nextLine_;
    int lastCurrentIndex_{-1};
};

} // namespace kivo::qt::lyrics
