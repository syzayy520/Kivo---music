// =============================================================================
// Kivo Music - NowPlayingControls.qml
// 职责: 沉浸视图的控制行 —— 随机/上一首/[播放]/下一首/循环（浅色字形）。
// 纯交互层：通过信号回报意图，由组合根接到 audioController。
// =============================================================================

import QtQuick
import "../transport"
import KivoMusic

Row {
    id: root
    property bool playing: false
    property int playbackModeValue: 0  // 0=Seq 1=Shuffle 2=RepeatOne 3=RepeatAll
    signal playPauseClicked()
    signal previousClicked()
    signal nextClicked()
    signal shuffleClicked()
    signal repeatClicked()

    spacing: 20

    TransportButton {
        kind: "shuffle"
        glyphColor: root.playbackModeValue === 1 ? Theme.accent : Theme.npText
        active: root.playbackModeValue === 1
        anchors.verticalCenter: parent.verticalCenter
        onClicked: root.shuffleClicked()
    }
    TransportButton {
        kind: "previous"
        glyphColor: Theme.npText
        anchors.verticalCenter: parent.verticalCenter
        onClicked: root.previousClicked()
    }
    NowPlayingPlayButton {
        playing: root.playing
        anchors.verticalCenter: parent.verticalCenter
        onClicked: root.playPauseClicked()
    }
    TransportButton {
        kind: "next"
        glyphColor: Theme.npText
        anchors.verticalCenter: parent.verticalCenter
        onClicked: root.nextClicked()
    }
    TransportButton {
        kind: root.playbackModeValue === 2 ? "repeat-one" : "repeat"
        glyphColor: (root.playbackModeValue === 2 || root.playbackModeValue === 3)
            ? Theme.accent : Theme.npText
        active: root.playbackModeValue === 2 || root.playbackModeValue === 3
        anchors.verticalCenter: parent.verticalCenter
        onClicked: root.repeatClicked()
    }
}
