// =============================================================================
// Kivo Music - MiniPlayer.qml
// 职责: 迷你播放器顶层窗口 —— 负责窗口帧、DragHandler 拖移、子组件编排。
// D1 修复: 原全局 MouseArea 拦截所有子控件事件; 改为 DragHandler+startSystemMove()
//          ——DragHandler 不阻塞子指针事件, close/seek/playback 按钮均可点击。
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import KivoMusic
import "../components/artwork"
import "mini"

ApplicationWindow {
    id: miniWindow
    width: 360
    height: 420
    visible: false
    title: "Kivo Music - Mini Player"
    color: "transparent"
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    // ── 系统原生拖移: DragHandler 不拦截子控件指针事件 ───────
    // 子控件(关闭/播放/进度等)点击不受影响; 拖空白区时交由 OS 完成窗口移动。
    DragHandler {
        acceptedButtons: Qt.LeftButton
        onActiveChanged: if (active) miniWindow.startSystemMove()
    }

    // ── 主卡片 ───────────────────────────────────────────────
    Rectangle {
        anchors.fill: parent
        anchors.margins: 8
        radius: 20
        color: Theme.panel
        border.color: Qt.rgba(0, 0, 0, 0.08)
        border.width: 0.5

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: "#30000000"
            shadowBlur: 0.8
            shadowVerticalOffset: 16
        }

        Column {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 16

            MiniHeader {
                width: parent.width
                onCloseRequested: miniWindow.hide()
            }

            AlbumArtwork {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 180; height: 180
                radiusValue: 12
                sourceUrl: audioController.coverArtPath
                playing: audioController.playing
                variant: audioController.coverVariantSeed
            }

            MiniTrackInfo {
                width: parent.width - 40
                anchors.horizontalCenter: parent.horizontalCenter
                title: audioController.title
                artist: audioController.artist
            }

            MiniProgressBar {
                width: parent.width - 40
                anchors.horizontalCenter: parent.horizontalCenter
                progress: audioController.progress
                onSeekRequested: audioController.seekTo(fraction)
            }

            MiniControls {
                anchors.horizontalCenter: parent.horizontalCenter
                playing: audioController.playing
                onPreviousRequested:  audioController.playPrevious()
                onPlayPauseRequested: audioController.togglePlayPause()
                onNextRequested:      audioController.playNext()
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 40

                Text {
                    text: audioController.elapsedText
                    color: Theme.faint
                    font.pixelSize: 10
                    font.weight: Font.Medium
                }
                Item { width: 1; height: 1 }
                Text {
                    text: audioController.durationText
                    color: Theme.faint
                    font.pixelSize: 10
                    font.weight: Font.Medium
                }
            }
        }
    }
}
