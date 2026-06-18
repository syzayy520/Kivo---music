// =============================================================================
// Kivo Music - NowPlayingPlayButton.qml
// 职责: 沉浸视图的主播放/暂停白圆钮（深色字形）。单一职责。
// =============================================================================

import QtQuick
import "../transport/glyphs"
import KivoMusic

Rectangle {
    id: root
    property bool playing: false
    signal clicked()

    width: 60
    height: 60
    radius: 30
    color: Theme.npButton
    scale: mouse.pressed ? 0.94 : 1.0
    Behavior on scale { NumberAnimation { duration: 90 } }

    // 共享字形(单一真源),深色字落在白圆上;大尺寸用更大的字形画布保持厚重感。
    PlayPauseGlyph {
        anchors.centerIn: parent
        width: 30
        height: 30
        playing: root.playing
        glyphColor: Theme.npButtonGlyph
        morph: true
    }
    MouseArea {
        id: mouse
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}
