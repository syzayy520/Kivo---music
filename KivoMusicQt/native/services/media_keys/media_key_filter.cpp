// =============================================================================
// Kivo Music Qt - services/media_keys/media_key_filter.cpp
// =============================================================================

#include "media_key_filter.hpp"

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace kivo::qt::services {

MediaKeyFilter::MediaKeyFilter(QObject* parent) : QObject(parent) {}

bool MediaKeyFilter::nativeEventFilter(
    const QByteArray& eventType,
    void* message,
    qintptr* result) noexcept {
#ifdef Q_OS_WIN
    if (eventType != "windows_generic_MSG") {
        return false;
    }
    const auto* msg = static_cast<const MSG*>(message);
    if (msg->message != WM_APPCOMMAND) {
        return false;
    }
    const int cmd = GET_APPCOMMAND_LPARAM(msg->lParam);
    switch (cmd) {
    case APPCOMMAND_MEDIA_PLAY_PAUSE:
        emit playPauseRequested();
        if (result) *result = 1;
        return true;
    case APPCOMMAND_MEDIA_NEXTTRACK:
        emit nextTrackRequested();
        if (result) *result = 1;
        return true;
    case APPCOMMAND_MEDIA_PREVIOUSTRACK:
        emit previousTrackRequested();
        if (result) *result = 1;
        return true;
    case APPCOMMAND_MEDIA_STOP:
        emit stopRequested();
        if (result) *result = 1;
        return true;
    default:
        return false;
    }
#else
    Q_UNUSED(eventType)
    Q_UNUSED(message)
    Q_UNUSED(result)
    return false;
#endif
}

} // namespace kivo::qt::services
