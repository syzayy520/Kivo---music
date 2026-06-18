// =============================================================================
// Kivo Music - windows/mini/MiniControls.qml
// 职责: 迷你播放器上一曲/播放暂停/下一曲控件行。使用 transport/glyphs 家族的字形。
// =============================================================================

import QtQuick
import QtQuick.Effects
import KivoMusic
import "../../components/transport/glyphs"

Row {
    id: root
    spacing: 24

    property bool playing: false
    signal previousRequested()
    signal playPauseRequested()
    signal nextRequested()

    // ── Previous ─────────────────────────────────────────────
    Rectangle {
        width: 36; height: 36; radius: 18
        color: prevMa.containsMouse ? Theme.panelSoft : "transparent"
        anchors.verticalCenter: parent.verticalCenter
        Behavior on color { ColorAnimation { duration: 120 } }

        PreviousGlyph {
            anchors.centerIn: parent
            width: 16; height: 16
            glyphColor: Theme.ink
        }

        MouseArea {
            id: prevMa
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.previousRequested()
        }
    }

    // ── Play / Pause ─────────────────────────────────────────
    Rectangle {
        width: 48; height: 48; radius: 24
        color: Theme.accent
        anchors.verticalCenter: parent.verticalCenter

        layer.enabled: true
        layer.effect: MultiEffect {
            shadowEnabled: true
            shadowColor: Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.35)
            shadowBlur: 0.5
            shadowVerticalOffset: 4
        }

        PlayPauseGlyph {
            anchors.centerIn: parent
            width: 22; height: 22
            playing: root.playing
            glyphColor: "#ffffff"
            morph: true
        }

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: root.playPauseRequested()
        }
    }

    // ── Next ─────────────────────────────────────────────────
    Rectangle {
        width: 36; height: 36; radius: 18
        color: nextMa.containsMouse ? Theme.panelSoft : "transparent"
        anchors.verticalCenter: parent.verticalCenter
        Behavior on color { ColorAnimation { duration: 120 } }

        NextGlyph {
            anchors.centerIn: parent
            width: 16; height: 16
            glyphColor: Theme.ink
        }

        MouseArea {
            id: nextMa
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: root.nextRequested()
        }
    }
}
