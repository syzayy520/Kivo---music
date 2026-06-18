// =============================================================================
// Kivo Music - windows/mini/MiniProgressBar.qml
// 职责: 迷你播放器进度条 —— 包装 SeekBar，传入小窗专用配色。
// =============================================================================

import QtQuick
import KivoMusic
import "../../components/common"

Item {
    id: root
    implicitHeight: 20

    property real progress: 0.0
    signal seekRequested(real fraction)

    SeekBar {
        anchors.fill: parent
        progress: root.progress
        trackColor: Theme.transportTrack
        fillColor: Theme.accent
        trackHeight: 3
        onSeekRequested: (position) => root.seekRequested(position)
    }
}
