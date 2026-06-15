// =============================================================================
// Kivo Music - MiniPlayer.qml
// 职责: 迷你播放器窗口（Apple Music 风格桌面常驻小窗）
// 显示: 封面 + 歌名/艺术家 + 播放/暂停 + 进度条 + 上一曲/下一曲
// =============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Effects
import "../tokens"

ApplicationWindow {
    id: miniWindow
    width: 360
    height: 420
    visible: false
    title: "Kivo Music - Mini Player"
    color: "#00000000"
    flags: Qt.Window | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint

    Theme { id: theme }

    // ── Drag to move ─────────────────────────────────────────
    MouseArea {
        anchors.fill: parent
        property point lastPos: Qt.point(0, 0)
        onPressed: lastPos = Qt.point(mouse.x, mouse.y)
        onPositionChanged: {
            miniWindow.x += mouse.x - lastPos.x;
            miniWindow.y += mouse.y - lastPos.y;
        }
    }

    // ── Main card ────────────────────────────────────────────
    Rectangle {
        anchors.fill: parent
        anchors.margins: 8
        radius: 20
        color: theme.panel
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

            // ── Close button ──────────────────────────────
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width

                Item { width: 30; height: 10 }  // spacer

                Text {
                    text: "Kivo Music"
                    color: theme.muted
                    font.pixelSize: 11
                    font.weight: Font.Medium
                    anchors.verticalCenter: parent.verticalCenter
                }

                Item { width: 1; height: 1 }  // spacer

                Rectangle {
                    width: 28; height: 28
                    radius: 14
                    color: mouseClose.containsMouse ? "#e5e5ea" : "#00000000"
                    anchors.verticalCenter: parent.verticalCenter

                    Text {
                        anchors.centerIn: parent
                        text: "×"
                        font.pixelSize: 16
                        color: theme.muted
                    }

                    MouseArea {
                        id: mouseClose
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: miniWindow.hide()
                    }
                }
            }

            // ── Album artwork ─────────────────────────────
            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 180; height: 180
                radius: 12
                color: "#e0e0e5"
                border.color: Qt.rgba(0, 0, 0, 0.06)
                border.width: 0.5

                layer.enabled: true
                layer.effect: MultiEffect {
                    shadowEnabled: true
                    shadowColor: "#18000000"
                    shadowBlur: 0.4
                    shadowVerticalOffset: 6
                }

                Text {
                    anchors.centerIn: parent
                    text: "🎵"
                    font.pixelSize: 48
                    opacity: 0.4
                }

                // Rotation while playing
                RotationAnimation on rotation {
                    running: audioController.playing
                    from: 0; to: 360
                    duration: 20000
                    loops: Animation.Infinite
                }
            }

            // ── Track info ────────────────────────────────
            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 2
                width: parent.width - 40

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: audioController.title
                    color: theme.text
                    font.pixelSize: 15
                    font.weight: Font.DemiBold
                    elide: Text.ElideRight
                    maximumLineCount: 1
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: audioController.artist
                    color: theme.muted
                    font.pixelSize: 12
                    elide: Text.ElideRight
                    maximumLineCount: 1
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            // ── Progress bar ──────────────────────────────
            Item {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 40
                height: 20

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width; height: 3
                    radius: 1.5
                    color: "#e5e5ea"
                }

                Rectangle {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width * audioController.progress; height: 3
                    radius: 1.5
                    color: theme.accent
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        var pos = mouse.x / parent.width;
                        audioController.seekTo(pos);
                    }
                }
            }

            // ── Controls ──────────────────────────────────
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 24

                // Previous
                Rectangle {
                    width: 36; height: 36; radius: 18
                    color: mousePrev.containsMouse ? "#f0f0f2" : "#00000000"
                    Text {
                        anchors.centerIn: parent
                        text: "⏮"
                        font.pixelSize: 16
                    }
                    MouseArea {
                        id: mousePrev
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: audioController.playPrevious()
                    }
                }

                // Play/Pause
                Rectangle {
                    width: 48; height: 48; radius: 24
                    color: theme.accent
                    Text {
                        anchors.centerIn: parent
                        text: audioController.playing ? "⏸" : "▶"
                        font.pixelSize: 18
                        color: "#ffffff"
                    }
                    MouseArea {
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: audioController.togglePlayPause()
                    }
                }

                // Next
                Rectangle {
                    width: 36; height: 36; radius: 18
                    color: mouseNext.containsMouse ? "#f0f0f2" : "#00000000"
                    Text {
                        anchors.centerIn: parent
                        text: "⏭"
                        font.pixelSize: 16
                    }
                    MouseArea {
                        id: mouseNext
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: audioController.playNext()
                    }
                }
            }

            // ── Time labels ───────────────────────────────
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 40
                Text {
                    text: audioController.elapsedText
                    color: theme.faint
                    font.pixelSize: 10
                    font.weight: Font.Medium
                }
                Item { width: 1; height: 1 }  // spacer
                Text {
                    text: audioController.durationText
                    color: theme.faint
                    font.pixelSize: 10
                    font.weight: Font.Medium
                }
            }
        }
    }
}
