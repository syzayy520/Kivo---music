// =============================================================================
// Kivo Music Qt - services/media_keys/media_key_filter.hpp
// 职责: 拦截 Windows WM_APPCOMMAND 系统媒体键消息, 转发为 Qt 信号。
// E10: 实现键盘媒体键(播放/暂停/下一首/上一首/停止)全局控制。
// =============================================================================

#pragma once

#include <QAbstractNativeEventFilter>
#include <QObject>

namespace kivo::qt::services {

class MediaKeyFilter final : public QObject, public QAbstractNativeEventFilter {
    Q_OBJECT

public:
    explicit MediaKeyFilter(QObject* parent = nullptr);
    ~MediaKeyFilter() override = default;

    bool nativeEventFilter(
        const QByteArray& eventType,
        void* message,
        qintptr* result) noexcept override;

signals:
    void playPauseRequested();
    void nextTrackRequested();
    void previousTrackRequested();
    void stopRequested();
};

} // namespace kivo::qt::services
