// =============================================================================
// Kivo Music - NowPlayingScrubber.qml
// 职责: 沉浸视图的进度条 + 已播/总时长。单一职责;通过 seek 信号回报拖动位置。
// 进度条复用共享的 SeekBar 基元(白底暗幕配色),不再手写轨道/命中映射。
// =============================================================================

import QtQuick
import "../common"
import KivoMusic

Column {
    id: root
    property real progress: 0
    property string elapsedText: ""
    property string durationText: ""
    signal seek(real position)  // 0..1

    spacing: 6

    SeekBar {
        width: parent.width
        progress: root.progress
        // 沉浸视图永远在暗色封面渐变之上 —— 白底白填充白把手。
        trackColor: Theme.npTrack
        fillColor: Theme.npButton
        handleColor: Theme.npButton
        trackHeight: 4
        onSeekRequested: function(position) { root.seek(position) }
    }

    Item {
        width: parent.width
        height: 14
        Text {
            anchors.left: parent.left
            text: root.elapsedText
            color: Theme.npTextFaint
            font.pixelSize: 11
            font.family: Theme.fontFamily
        }
        Text {
            anchors.right: parent.right
            text: root.durationText
            color: Theme.npTextFaint
            font.pixelSize: 11
            font.family: Theme.fontFamily
        }
    }
}
