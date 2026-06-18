// =============================================================================
// Kivo Music - NowPlayingCollapseButton.qml
// 职责: 沉浸视图左上角的「收起」圆钮。单一职责。
// =============================================================================

import QtQuick
import KivoMusic

Rectangle {
    id: root
    signal clicked()

    width: 40
    height: 40
    radius: 20
    color: mouse.containsMouse ? Theme.npControlHover : Theme.npControlBg
    Behavior on color { ColorAnimation { duration: 120 } }

    Text {
        anchors.centerIn: parent
        text: "⌄"
        color: Theme.npText
        font.pixelSize: 24
    }
    MouseArea {
        id: mouse
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onClicked: root.clicked()
    }
}
