// =============================================================================
// Kivo Music Qt - playback_mode.hpp
// Playback Mode Enumeration (Sequential / Shuffle / Repeat)
// =============================================================================

#pragma once

#include <QObject>
#include <QRandomGenerator>

namespace kivo::qt::audio_bridge {

class PlaybackMode : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString modeString READ modeString NOTIFY modeChanged)

public:
    enum class Mode {
        Sequential,   // 顺序播放
        Shuffle,      // 随机播放
        RepeatOne,    // 单曲循环
        RepeatAll     // 列表循环
    };
    Q_ENUM(Mode)

    explicit PlaybackMode(QObject* parent = nullptr);
    ~PlaybackMode() override = default;

    // Mode control
    Q_INVOKABLE void setMode(Mode mode);
    Q_INVOKABLE Mode mode() const { return currentMode_; }
    Q_INVOKABLE void toggleMode();
    Q_INVOKABLE QString modeString() const;

    // Next/Previous logic
    Q_INVOKABLE int nextIndex(int currentIndex, int totalCount);
    Q_INVOKABLE int previousIndex(int currentIndex, int totalCount);

signals:
    void modeChanged();

private:
    Mode currentMode_ = Mode::Sequential;
    QRandomGenerator randomGenerator_;

    int shuffleIndex(int totalCount);
};

} // namespace kivo::qt::audio_bridge
