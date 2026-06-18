// =============================================================================
// Kivo Music - NowPlayingView.qml
// 职责: 沉浸式正在播放视图的「组合根」—— 只负责组装 nowplaying/ 家族 + 接线到
// audioController + 取色回退。每个零件是独立单职责文件,本文件不含实现细节。
// 超越 Apple Music: 取色封面氛围 + AM 不显示的信号链真相条。
// =============================================================================

import QtQuick
import "../components/nowplaying"
import "../components/truth"
import KivoMusic

Item {
    id: view
    property bool open: false
    // The audio-truth signal-path chain, expanded from the truth capsule.
    property bool truthExpanded: false

    visible: opacity > 0.01
    opacity: open ? 1.0 : 0.0
    Behavior on opacity { NumberAnimation { duration: 280; easing.type: Easing.OutCubic } }

    // 模态:打开时抓焦点,支持 Esc 收起(与 PlayQueueDrawer 一致)。Esc 先收信号链,再收视图。
    focus: open
    onOpenChanged: {
        if (open) forceActiveFocus()
        else truthExpanded = false  // never linger when collapsed
    }
    Keys.onEscapePressed: function(event) {
        if (view.truthExpanded) view.truthExpanded = false
        else view.open = false
        event.accepted = true
    }

    // Artwork-extracted color; fall back to a neutral dark when no/black cover.
    readonly property color artColor: audioController.coverArtColor
    readonly property bool artColorValid: (artColor.r + artColor.g + artColor.b) > 0.08
    readonly property color baseColor: artColorValid ? artColor : Qt.rgba(0.16, 0.16, 0.18, 1)

    // 收起时不喂封面 url —— Image 在组件创建时就会开始解码(visible 只挡绘制不挡加载),
    // 这里仅在视图打开时才加载,避免常驻后台白白解码/占显存。
    NowPlayingBackground {
        anchors.fill: parent
        baseColor: view.baseColor
        source: view.open ? audioController.coverArtPath : ""
    }

    MouseArea { anchors.fill: parent; hoverEnabled: true }  // swallow stray clicks

    NowPlayingCollapseButton {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 24
        anchors.leftMargin: 24
        onClicked: view.open = false
    }

    Column {
        anchors.centerIn: parent
        width: 360
        spacing: 24

        // Subtle entrance: content rises + scales in as the view opens.
        opacity: view.open ? 1.0 : 0.0
        scale: view.open ? 1.0 : 0.965
        transform: Translate {
            y: view.open ? 0 : 18
            Behavior on y { NumberAnimation { duration: 360; easing.type: Easing.OutCubic } }
        }
        Behavior on opacity { NumberAnimation { duration: 320; easing.type: Easing.OutCubic } }
        Behavior on scale { NumberAnimation { duration: 360; easing.type: Easing.OutCubic } }

        NowPlayingArtwork {
            anchors.horizontalCenter: parent.horizontalCenter
            size: 340
            source: view.open ? audioController.coverArtPath : ""
            playing: audioController.playing
        }

        NowPlayingMeta {
            width: parent.width
            title: audioController.title
            artist: audioController.artist
        }

        NowPlayingScrubber {
            width: parent.width
            progress: audioController.progress
            elapsedText: audioController.elapsedText
            durationText: audioController.durationText
            onSeek: function(position) { audioController.seekTo(position) }
        }

        NowPlayingControls {
            anchors.horizontalCenter: parent.horizontalCenter
            playing: audioController.playing
            playbackModeValue: audioController.playbackModeValue
            onPlayPauseClicked: audioController.togglePlayPause()
            onPreviousClicked: audioController.playPrevious()
            onNextClicked: audioController.playNext()
            onShuffleClicked: audioController.toggleShuffle()
            onRepeatClicked: audioController.cycleRepeat()
        }

        NowPlayingTruthStrip {
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: view.truthExpanded = true
        }
    }

    // ── Expanded signal-path chain — the moat, brought into the immersive view.
    //    Dims the artwork, focuses the full Source→Decode→Engine→Output chain.
    Item {
        anchors.fill: parent
        visible: view.truthExpanded || scrim.opacity > 0.01
        z: 50

        Rectangle {
            id: scrim
            anchors.fill: parent
            color: "#aa000000"
            opacity: view.truthExpanded ? 1.0 : 0.0
            Behavior on opacity { NumberAnimation { duration: 200 } }
            MouseArea { anchors.fill: parent; onClicked: view.truthExpanded = false }
        }

        SignalPathPanel {
            anchors.centerIn: parent
            opacity: view.truthExpanded ? 1.0 : 0.0
            scale: view.truthExpanded ? 1.0 : 0.95
            Behavior on opacity { NumberAnimation { duration: 220; easing.type: Easing.OutCubic } }
            Behavior on scale { NumberAnimation { duration: 280; easing.type: Easing.OutCubic } }
        }
    }
}
